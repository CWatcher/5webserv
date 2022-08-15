#include "SimpleHandler.hpp"

const std::pair<std::string, SimpleHandler::handler>    SimpleHandler::handlers_init_list_[] =
{
    std::make_pair("GET", &SimpleHandler::get),
    std::make_pair("POST", &SimpleHandler::post),
    std::make_pair("DELETE", &SimpleHandler::del),
};

const std::map<std::string, SimpleHandler::handler> SimpleHandler::handlers_(handlers_init_list_, handlers_init_list_ + sizeof(handlers_init_list_)/ sizeof(handlers_init_list_[0]));

void    SimpleHandler::makeResponse()
{
    try
    {
        validateRequest();
        if (location_.redirect.second.empty())
        {
            path_ = location_.root + request_.uri();
            std::map<std::string, SimpleHandler::handler>::const_iterator handler = handlers_.find(request_.method());
            (this->*handler->second)();
        }
        else
            redirect(location_.redirect.first);
    }
    catch(const SimpleHandler::HTTPError& e)
    {
        error(e.status());
    }
    catch(const std::exception& e)
    {
        error(INTERNAL_SERVER_ERROR);
    }
}

void    SimpleHandler::validateRequest()
{
    if (request_.method().empty() && request_.uri().empty() && request_.http().empty())
        throw SimpleHandler::HTTPError(BAD_REQUEST);
    if (request_.http() != "HTTP/1.1")
        throw SimpleHandler::HTTPError(HTTP_VERSION_NOT_SUPPORTED);
    if (handlers_.find(request_.method()) == handlers_.end())
        throw SimpleHandler::HTTPError(NOT_IMPLEMENTED);
    if (location_.methods.find(request_.method()) == location_.methods.end())
        throw SimpleHandler::HTTPError(METHOD_NOT_ALLOWED);
    //TO DO проверить заголовки
        // throw SimpleHandler::HTTPError(BAD_REQUEST);
}

void    SimpleHandler::get()
{

}

void    SimpleHandler::post()
{

}

void    SimpleHandler::del()
{

}

void    SimpleHandler::error(HTTPStatus)
{
    // TO DO добавить обработку кастомных ошибок

}

void    SimpleHandler::redirect(unsigned)
{

}
