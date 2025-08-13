#include "yaccs/utils.hpp"
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <string>
#include <unistd.h>
#include <sys/wait.h>

std::string extract_filename(const std::string& path)
{
    auto last_slash_pos{path.rfind("/")};

    if (last_slash_pos == path.back()) {
        last_slash_pos = 0;
    } else {
        ++last_slash_pos;
    }

    auto filename_ext{path.substr(last_slash_pos, path.back())};
    auto last_dot_pos{filename_ext.rfind(".")};

    if (last_dot_pos == path.back()) {
        return filename_ext;
    } else {
        return filename_ext.substr(0, last_dot_pos);
    }
}

bool invoke_spirv_as(const std::string& spvasm, const std::string& out_file)
{
    auto child_pid{fork()};
    if (child_pid == 0) {
        // child process
        execlp("spirv-as", "spirv-as", spvasm.c_str(), "--target-env", "vulkan1.4", "-o", out_file.c_str(), NULL);
    } else {
        // parent process
        int status{0};
        waitpid(child_pid, &status, 0);
    }
    return true;
}

bool remove_file(const std::string& filename)
{
    remove(filename.c_str());
    return true;
}
