#include "simple_compute.h"

#include <chrono>
#include <iomanip>

float randfloat() {
  return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

template<typename Timestamp>
auto timer_report(const Timestamp from, const Timestamp to) {
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(to - from);
  return duration.count();
}

void smoothing_cpu(const std::vector<float> &source, std::vector<float> &output)
{
  constexpr size_t KERNEL_SIZE = 7;
  constexpr size_t KERNEL_TO_LEFT = 3;
  constexpr size_t KERNEL_TO_RIGHT = KERNEL_SIZE - KERNEL_TO_LEFT;

  for (size_t i = 0; i < source.size(); ++i) {
    size_t left = i < KERNEL_TO_LEFT ? 0 : i - KERNEL_TO_LEFT;
    size_t right = i + KERNEL_TO_RIGHT >= source.size() ? source.size() - 1 : i + KERNEL_TO_RIGHT;

    float window_sum = 0.0f;
    for (size_t j = left; j <= right; ++j) {
      window_sum += source[j];
    }

    // output[i] = -window_sum / (float) KERNEL_SIZE;
    // output[i] = source[i];
    output[i] = source[i] -window_sum / (float) KERNEL_SIZE;
  }

  for (size_t i = 0; i < output.size(); ++i) {
    std::cout << output[i] << ' ';
  }
  std::cout << '\n';
}

void smoothing_gpu(std::shared_ptr<SimpleCompute> &app) {
  app->Execute();
}

int main()
{
  // std::cout << time(0) << std::endl;
  srand (static_cast <unsigned> (1702133823));

  std::cout << std::setprecision(20);

  constexpr int LENGTH = 10; // больше не влезает
  constexpr int WGSIZE = 256;
  constexpr int WGCOUNT = LENGTH / WGSIZE + 1;

  constexpr int VULKAN_DEVICE_ID = 0;
  
  std::shared_ptr<SimpleCompute> app = std::make_unique<SimpleCompute>(LENGTH);
  if(app == nullptr)
  {
    std::cout << "Can't create render of specified type" << std::endl;
    return 1;
  }

  app->InitVulkan(nullptr, 0, VULKAN_DEVICE_ID);

  std::vector<float> data(LENGTH);
  for (size_t i = 0; i < data.size(); ++i) {
    data[i] = randfloat() + 1.0;
  }
  std::vector<float> cpu_output = data;

  // CPU
  std::cout << "CPU execution...\n";

  auto  cpu_start  = std::chrono::high_resolution_clock::now();
  smoothing_cpu(data, cpu_output);
  auto  cpu_finish = std::chrono::high_resolution_clock::now();
  std::cout << "Finished, time elapsed: " << timer_report(cpu_start, cpu_finish) << "ms" << '\n';

  float cpu_result = 0;
  for (auto x : cpu_output) {
    cpu_result += x;
  }
  std::cout << "Result: " << cpu_result << '\n';

  // GPU
  std::cout << "GPU execution...\n";
  app->Setup(WGCOUNT);
  app->FillData(data);

  auto  gpu_start  = std::chrono::high_resolution_clock::now();
  smoothing_gpu(app);
  auto  gpu_finish = std::chrono::high_resolution_clock::now();
  std::cout << "Finished, time elapsed: " << timer_report(gpu_start, gpu_finish) << "ms" << '\n';

  float gpu_result = app->GetResults();
  std::cout << "Result: " << gpu_result << '\n';

  // Results
  if (abs(cpu_result - gpu_result) > 0.0001) {
    std::cout << "Results are not equal!\n";
    // Это нормальная ситуация при нашем количестве обьектов
  } else {
    std::cout << "Results are equal: " << cpu_result << "\n";
    // А вот это не нормальная
  }

  return 0;
}
