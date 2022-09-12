
#include "old_handlers/base/HandlerTask.hpp"
#include "old_handlers/HeaderParser.hpp"
#include "old_handlers/HeaderValidator.hpp"
#include "old_handlers/FileReader.hpp"
#include "old_handlers/CGIReader.hpp"
#include "old_handlers/HeaderGenerator.hpp"
#include "utils/log.hpp"

// ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^
// | | | | | | | | | | | | | | | | | | |
// Handlers execution from bottom to top
// | | | | | | | | | | | | | | | | | | |
// | | | | | | | | | | | | | | | | | | |

namespace handlers
{
    // LAST HANDLER
    HeaderGenerator header_generator;
    CGIReader       cgi_reader(&header_generator);
    FileReader      file_reader(&cgi_reader);
    HeaderValidator header_validator(&file_reader);
    HeaderParser    header_parser(&header_validator);
    IHandler        *start_handler = &header_parser;
    // FIRST HANDLER

    void *run(void *task_void)
    {
        // TODO: проверить, что в ответе нет статуса 500
        HandlerTask *task = reinterpret_cast<HandlerTask *>(task_void);
        logger::info << task->session->request().start_line() << logger::end;

        // TODO: need to find and pass proper location or server config
        start_handler->handle(task->location, task->session->request(), task->session->response());

        logger::debug << "I know you came from port: " << ntohs(task->session->port()) << logger::end;
        logger::debug << "Your server config:\n" << task->location << logger::end;

        task->session->setStateToWrite();
        logger::debug << "handlers: run: task completed" << logger::end;

        delete task;
        return NULL;
    }
}
