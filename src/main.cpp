#include "backend.hpp"
#include "gui.hpp"
#include "sharedData.hpp"

#include <thread>

int main()
{
    sharedData sd;
    std::thread backend_t(run_backend, std::ref(sd));
    run_gui(sd);

    if (backend_t.joinable())
        backend_t.join();

    return 0;
}
