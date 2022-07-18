
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
    IHandler        *runner = &header_parser;
    // FIRST HANDLER
    
    void *run(void *task_void)
    {
        SocketSession *task = reinterpret_cast<SocketSession *>(task_void);

        runner->handle(task->input, task->output);
        task->prepareForWrite();
        logger::debug << "handlers: run: task completed" << logger::end;
        return NULL;
    }
}
