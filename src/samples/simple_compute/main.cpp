#include "simple_compute.h"

float randfloat() {
  return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

template<typename Timestamp>
auto timer_report(const Timestamp from, const Timestamp to) {
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(to - from);
  return duration.count();
}

float computeOnGpu(std::shared_ptr<SimpleCompute> &app) {
  app->Execute();
  return app->GetResults();
}

int main()
{
  srand (static_cast <unsigned> (time(0)));

  constexpr int LENGTH = 10000;
  constexpr int WGSIZE = 256;
  constexpr int VULKAN_DEVICE_ID = 0;

  constexpr int WGCOUNT = LENGTH / WGSIZE + static_cast<bool>(LENGTH % WGSIZE);
  
  std::shared_ptr<SimpleCompute> app = std::make_unique<SimpleCompute>(LENGTH);
  if(app == nullptr)
  {
    std::cout << "Can't create render of specified type" << std::endl;
    return 1;
  }

  app->InitVulkan(nullptr, 0, VULKAN_DEVICE_ID);

  std::vector<float> data(LENGTH);
  for (size_t i = 0; i < data.size(); ++i) {
    data[i] = randfloat();
  }

  // CPU
  std::cout << "CPU execution...\n";
  auto  cpu_start  = std::chrono::high_resolution_clock::now();
  float cpu_result = computeOnCpu(data);
  auto  cpu_finish = std::chrono::high_resolution_clock::now();
  std::cout << "Finished, time elapsed: " << timer_report(cpu_start, cpu_finish) << "ms" << '\n';
  std::cout << "Result: " << cpu_result << '\n';

  // GPU
  app->Setup(WGCOUNT);
  // app->FillData(data);

  // std::cout << "GPU execution...\n";
  // auto  gpu_start  = std::chrono::high_resolution_clock::now();
  // float gpu_result = computeOnGpu(app);
  // auto  gpu_finish = std::chrono::high_resolution_clock::now();
  // std::cout << "Finished, time elapsed: " << timer_report(gpu_start, gpu_finish) << "ms" << '\n';
  // std::cout << "Result: " << gpu_result << '\n';

  // // Results
  // if (abs(cpu_result - gpu_result) > 0.0001) {
  //   std::cout << "Results are not equal!\n";
  //   std::cout << "CPU result: " << cpu_result << "\n";
  //   std::cout << "GPU result: " << gpu_result << "\n";
  // } else {
  //   std::cout << "Results are equal: " << cpu_result << "\n";
  // }

  return 0;
}
