
#include "handlers/HeaderParser.hpp"
#include "handlers/HeaderValidator.hpp"
#include "handlers/FileReader.hpp"
#include "handlers/CGIReader.hpp"
#include "handlers/HeaderGenerator.hpp"

// ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^
// | | | | | | | | | | | | | | | | | |
// Handlers execute from bottom to top
// | | | | | | | | | | | | | | | | | |
// | | | | | | | | | | | | | | | | | |

namespace handlers
{
// LAST HANDLER
HeaderGenerator header_generator;
CGIReader       cgi_reader(&header_generator);
FileReader      file_reader(&cgi_reader);
HeaderValidator header_validator(&file_reader);
HeaderParser    header_parser(&header_validator);
IHandler        *starter = &header_parser;
// FIRST HANDLER
}
