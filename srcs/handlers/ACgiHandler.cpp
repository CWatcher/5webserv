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
#include <climits>
#include <fcntl.h>

ACgiHandler::ACgiHandler(const Location& loc, const HTTPRequest& req, in_addr_t s_ip, in_port_t s_port, const in_addr& remote_addr) :
    AHandler(loc, req),
    file_info_(location_.root + pure_uri_),
    cgi_pid_(0),
    server_ip_(s_ip),
    server_port_(s_port),
    remote_addr_(remote_addr),
    cgi_out_file_(NULL)
{
    std::map<std::string, std::string>::const_iterator  cgi = location_.cgi.find(file_info_.type());

    if (cgi != location_.cgi.end())
        cgi_path_ = cgi->second;
    cgi_in_pipe_[0] = cgi_in_pipe_[1] = -1;
}

ACgiHandler::~ACgiHandler()
{
    if (cgi_in_pipe_[0] != -1)
        ::close(cgi_in_pipe_[0]);
    if (cgi_in_pipe_[1] != -1)
        ::close(cgi_in_pipe_[1]);
    if (cgi_out_file_ != NULL)
        ::fclose(cgi_out_file_);
    if (cgi_pid_ != 0)
    {
        ::kill(cgi_pid_, SIGKILL);
        ::waitpid(cgi_pid_, NULL, 0);
    }
}

void    ACgiHandler::runCgi(HTTPResponse& response)
{
    cgi_out_file_ = ::tmpfile();
    if (cgi_out_file_ == NULL)
        throw HTTPError(HTTPStatus::INTERNAL_SERVER_ERROR);

    if (request_.method() == "POST")
    {
        if (::pipe(cgi_in_pipe_))
            throw HTTPError(HTTPStatus::INTERNAL_SERVER_ERROR);
    }

    cgi_pid_ = ::fork();
    if (cgi_pid_ == -1)
        throw HTTPError(HTTPStatus::INTERNAL_SERVER_ERROR);
    else if (cgi_pid_ == 0)
        forkCgi();
    else
        parentCgi(response);
}

void    ACgiHandler::forkCgi() const
{
    std::vector<char*>  envp;
    std::vector<char*>  argv;

    if (request_.method() == "POST")
    {
        ::close(cgi_in_pipe_[1]);
        if (dup2(cgi_in_pipe_[0], STDIN_FILENO) == -1)
            ::raise(SIGKILL);
        ::close(cgi_in_pipe_[0]);
    }

    if (::dup2(::fileno(cgi_out_file_), STDOUT_FILENO) == -1)
        ::raise(SIGKILL);
    ::fclose(cgi_out_file_);

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
    char                        real_path[PATH_MAX];

    tmp = request_.getHeaderValue("Content-Length");
    if (!tmp.empty())
        envp_data.push_back("CONTENT_LENGTH=" + tmp);

    tmp = request_.getHeaderValue("Content-Type");
    if (!tmp.empty())
        envp_data.push_back("CONTENT_TYPE=" + tmp);

    envp_data.push_back("GATEWAY_INTERFACE=CGI/1.1");

    if (!path_info_.empty())
    {
        envp_data.push_back("PATH_INFO=" + path_info_);
        if (::realpath((location_.root + path_info_).c_str(), real_path) != NULL)
        {
            tmp = real_path;
            envp_data.push_back("PATH_TRANSLATED=" + tmp);
        }
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

    converter << ntohs(server_port_);
    envp_data.push_back("SERVER_PORT=" + converter.str());

    envp_data.push_back("SERVER_PROTOCOL=HTTP/1.1");
    envp_data.push_back("SERVER_SOFTWARE=webserv");

    tmp = ::realpath(file_info_.path().c_str(), real_path);
    envp_data.push_back("SCRIPT_FILENAME=" + tmp);

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

void    ACgiHandler::parentCgi(HTTPResponse& response) const
{
    int         status;
    char        *cgi_data;
    struct stat cgi_stat;

    if (request_.method() == "POST")
    {
        // static size_t   bytes_sent = 0;
        // ssize_t         w;

        ::close(cgi_in_pipe_[0]);
        ::write(cgi_in_pipe_[1], request_.body(), request_.body_size());
        // if (w == -1)
        //     throw HTTPError(HTTPStatus::BAD_GATEWAY);
        // bytes_sent += w;
        // if (bytes_sent != request_.body_size())
        //     return;
        ::close(cgi_in_pipe_[1]);
    }


    ::waitpid(cgi_pid_, &status, 0);
    if (!WIFEXITED(status))
        throw HTTPError(HTTPStatus::BAD_GATEWAY);

    ::fstat(::fileno(cgi_out_file_), &cgi_stat);
    cgi_data = reinterpret_cast<char *>(::mmap(NULL, cgi_stat.st_size, PROT_READ, MAP_SHARED, fileno(cgi_out_file_), 0));
    if (cgi_data == NULL)
        throw HTTPError(HTTPStatus::INTERNAL_SERVER_ERROR);
    makeCgiResponse(cgi_data, cgi_stat.st_size, response);
    ::munmap(cgi_data, cgi_stat.st_size);
}

const char*  ACgiHandler::getCgiBody(const char* cgi_data, size_t n)
{
    std::string   delim = "\n\n";
    const char*   cgi_body = std::search(cgi_data, cgi_data + n, delim.begin(), delim.end());

    if (cgi_body != cgi_data + n)
        return cgi_body + 2;

    delim = "\r\n\r\n";
    cgi_body = std::search(cgi_data, cgi_data + n, delim.begin(), delim.end());
    if (cgi_body != cgi_data + n)
        return cgi_body + 4;

    ::munmap(const_cast<char*>(cgi_data), n);
    throw HTTPError(HTTPStatus::BAD_GATEWAY);
}

void    ACgiHandler::makeCgiResponse(const char* cgi_data, size_t n, HTTPResponse& response) const
{
    const char*         cgi_body = getCgiBody(cgi_data, n);
    size_t              content_length = cgi_data + n - cgi_body;
    std::string         cgi_header(cgi_data, cgi_body);
    std::string         status = "200 OK";

    size_t              first = 0;
    size_t              middle = cgi_header.find(':');
    size_t              last = cgi_header.find('\n', middle);

    while (middle != std::string::npos)
    {
        std::string key = cgi_header.substr(first, middle - first);
        ++middle;
        std::string value = cgi_header.substr(middle, last - middle);

        first = ++last;
        middle = cgi_header.find(':', last);
        last = cgi_header.find('\n', middle);

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
            ss >> content_length;
        }
    }
    response.setContentLength(content_length);
    response.buildResponse(cgi_body, cgi_body + content_length, request_.method() != "HEAD", status);
}
