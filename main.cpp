#include <cstring>
#include <filesystem>
#include <iostream>

#include <cli.h>
#include <compose.h>
#include <config.h>

#include "server.h"

int main(int argc, const char* argv[])
{
    int result = 0;
    compose the_compose {
        ETC_PATH PROJECT_NAME "/command.pipe" };
    config the_config { ETC_PATH PROJECT_NAME "/config.json", the_compose };
    bool args = false;
    try
    {
        Cli::parse(argc, argv,
        {
            {
                { "--version" },
                Cli::Handler({ [&](const std::vector<std::string>&)
                {
                    std::cout << PROJECT_NAME << ' ' << PROJECT_VERSION <<
                        std::endl;
                    args = true;
                }, 0, 0})
            },
        });
    }
    catch(const std::exception& e)
    {
        result = -1;
        args = true;
        std::cerr << e.what() << std::endl;
    }
    if (!args)
    {
        std::cout << "--- Hello" << std::endl;
        server the_server{ ROOT, the_config, the_compose };
        the_compose.run(std::chrono::seconds(1));
        std::cout << "--- Bye" << std::endl;
    }
    return result;
}
