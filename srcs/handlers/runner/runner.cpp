
#include "handlers/base/HandlerTask.hpp"
#include "handlers/HeaderParser.hpp"
#include "handlers/HeaderValidator.hpp"
#include "handlers/FileReader.hpp"
#include "handlers/CGIReader.hpp"
#include "handlers/HeaderGenerator.hpp"
#include "SocketSession.hpp"

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
        HandlerTask *task = reinterpret_cast<HandlerTask *>(task_void);

        start_handler->handle(task->session->input, task->session->output);

        // need to pass server config
        logger::debug << "I know you came from port:" << ntohs(task->session->from_listen_address.second) << logger::end;
        logger::debug << "Your server config:\n" << task->config << logger::end;

        task->session->prepareForWrite();
        logger::debug << "handlers: run: task completed" << logger::end;

        delete task;
        return NULL;
    }
}
