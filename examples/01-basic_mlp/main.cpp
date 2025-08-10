#include <iostream>
#include <vector>

#define COV_VULKAN_VALIDATION
#define COV_IMPLEMENTATION
#include "cov.hpp"


int main()
{
    std::string shader_path{"model.spv"};

    cov::App::init("BasicMlpModel");

    {
        // core code
        auto instance{cov::App::new_instance()};
        instance.load_shader(shader_path);

        std::vector<float> input{1.0, 2.0, 3.0, 4.0};
        std::vector<float> output(4);

        instance.set_inputs({
            {input.data(), input.size() * sizeof(input.at(0))},
        });
        instance.def_output(output.size() * sizeof(output.at(0)));

        if (!instance.execute({32, 32, 1})) {
            std::cerr << "Execute shader program failed\n";
        }
        // instance.get_output(output.data(), output.size() * sizeof(output.at(0)));
        // The instance will be automatically destroy here.
        
        std::cout << "Output: ";
        for (auto it: output) {
            std::cout << it << " ";
        }
        std::cout << "\n";
    }


    return 0;
}
