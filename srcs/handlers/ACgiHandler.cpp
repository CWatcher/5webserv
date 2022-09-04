#include "handlers/ACgiHandler.hpp"
#include "utils/log.hpp"
#include "utils/string.hpp"

#include <sstream>
#include <algorithm>
#include <unistd.h>
#include <signal.h>
#include <cstring>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/mman.h>

ACgiHandler::ACgiHandler(const Location& loc, const HTTPRequest& req, in_addr_t s_ip, in_port_t s_port, const in_addr& remote_addr) :
    AHandler(loc, req),
    server_ip_(s_ip),
    server_port_(s_port),
    remote_addr_(remote_addr),
    file_info_(location_.root + pure_uri_)
{
    std::map<std::string, std::string>::const_iterator  cgi = location_.cgi.find(file_info_.type());

    if (cgi != location_.cgi.end())
        cgi_path_ = cgi->second;
}

void    ACgiHandler::cgi(HTTPResponse& response) const
{
    pid_t                       cgi_pid;
    FILE*                       cgi_out_file = ::tmpfile();

    if (cgi_out_file == NULL)
        throw HTTPError(HTTPStatus::INTERNAL_SERVER_ERROR);

    cgi_pid = ::fork();
    if (cgi_pid == -1)
        throw HTTPError(HTTPStatus::INTERNAL_SERVER_ERROR);
    else if (cgi_pid == 0)
        runCgi(cgi_out_file);
    else
        waitCgi(cgi_pid, cgi_out_file, response);
}

void    ACgiHandler::runCgi(FILE* cgi_out_file) const
{
    std::vector<char*>  envp;
    std::vector<char*>  argv;

    if (::dup2(::fileno(cgi_out_file), STDOUT_FILENO) == -1)
        ::raise(SIGKILL);
    ::fclose(cgi_out_file);

    makeCgiEnv(envp);
    argv.push_back(::strdup(cgi_path_.c_str()));
    argv.push_back(::strdup(file_info_.path().c_str()));
    argv.push_back(NULL);

    ::execve(cgi_path_.c_str(), &*argv.begin(), &*envp.begin());

    logger::error << logger::cerror << logger::end;
    ::raise(SIGKILL);
}

void    ACgiHandler::makeCgiEnv(std::vector<char*>& envp) const
{
    std::vector<std::string>    envp_data;
    std::stringstream           converter;
    std::string                 tmp;

    tmp = request_.getHeaderValue("Content-Length");
    if (!tmp.empty())
        envp_data.push_back(tmp + "CONTENT_LENGTH=");

    tmp = request_.getHeaderValue("Content-Type");
    if (!tmp.empty())
        envp_data.push_back("CONTENT_TYPE=" + tmp);

    envp_data.push_back("GATEWAY_INTERFACE=CGI/1.1");

    if (!path_info_.empty())
    {
        envp_data.push_back("PATH_INFO=" + path_info_);
        envp_data.push_back("PATH_TRANSLATED=" + location_.root + path_info_);
    }

    if (!query_string_.empty())
        envp_data.push_back("QUERY_STRING=" + query_string_);

    tmp = "REMOTE_ADDR=";
    envp_data.push_back(tmp + ::inet_ntoa(remote_addr_));

    envp_data.push_back("REQUEST_METHOD=" + request_.method());
    envp_data.push_back("SCRIPT_NAME=" + pure_uri_);

    tmp = "SERVER_NAME=";
    in_addr server_addr;
    server_addr.s_addr = server_ip_;
    envp_data.push_back(tmp + ::inet_ntoa(server_addr));

    converter << ::ntohs(server_port_);
    envp_data.push_back("SERVER_PORT=" + converter.str());

    envp_data.push_back("SERVER_PROTOCOL=HTTP/1.1");
    envp_data.push_back("SERVER_SOFTWARE=webserv");

    for (std::map<std::string, std::string>::const_iterator it = request_.header().begin();
        it != request_.header().end(); ++it)
    {
        tmp = it->first;
        std::replace(tmp.begin(), tmp.end(), '-', '_');
        std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);
        envp_data.push_back("HTTP_" + tmp + '=' + it->second);
    }

    for (std::vector<std::string>::const_iterator it = envp_data.begin(); it != envp_data.end(); ++it)
        envp.push_back(::strdup(it->c_str()));
    envp.push_back(NULL);
}

void    ACgiHandler::waitCgi(pid_t cgi_pid, FILE* cgi_out_file, HTTPResponse& response) const
{
    int         status;
    char        *cgi_data;
    struct stat cgi_stat;

    ::waitpid(cgi_pid, &status, 0);
    if (!WIFEXITED(status))
    {
        fclose(cgi_out_file);
        throw HTTPError(HTTPStatus::BAD_GATEWAY);
    }
    fflush(cgi_out_file);
    ::fstat(::fileno(cgi_out_file), &cgi_stat);
    cgi_data = reinterpret_cast<char *>(::mmap(NULL, cgi_stat.st_size, PROT_READ, MAP_SHARED, fileno(cgi_out_file), 0));
    ::fclose(cgi_out_file);
    if (cgi_data == NULL)
        throw HTTPError(HTTPStatus::INTERNAL_SERVER_ERROR);
    makeCgiResponse(response, cgi_data, cgi_stat.st_size);
    ::munmap(cgi_data, cgi_stat.st_size);
}

void    ACgiHandler::makeCgiResponse(HTTPResponse& response, const char* cgi_data, size_t n) const
{
    const std::string   delim = "\n\n";
    const char*         cgi_header_end = std::search(cgi_data, cgi_data + n, delim.begin(), delim.end());
    std::string         cgi_header;
    size_t              first, last;
    std::string         status = "200 OK";

    if (cgi_header_end == cgi_data + n)
    {
        ::munmap(const_cast<char*>(cgi_data), n);
        throw (HTTPStatus::BAD_GATEWAY);
    }

    cgi_header.append(cgi_data, cgi_header_end);
    first = 0;
    last = cgi_header.find(':');
    while (last != std::string::npos)
    {
        std::string key = cgi_header.substr(first, last);
        std::string value;

        first = last + 1;
        last = cgi_header.find(last, '\n');
        value = cgi_header.substr(first, last);
        strTrim(key);
        strTrim(value);
        if (strLowerCaseCopy(key) == "status")
            status = value;
        else
            response.addHeader(key, value);
        if (strLowerCaseCopy(key) == "content-length")
        {
            std::stringstream   ss;

            ss << value;
            ss >> n;
        }
    }
    response.setContentLength(n);
    response.buildResponse(cgi_data, cgi_data + n, request_.method() != "HEAD", status);
}
