#include "backend.hpp"
#include "gui.hpp"
#include "sharedData.hpp"

#include <thread>

int main()
{
    sharedData sh_data;
    std::thread backend_t(run_backend, std::ref(sh_data));
    run_gui(sh_data);

    if (backend_t.joinable())
        backend_t.join();

    return 0;
}
