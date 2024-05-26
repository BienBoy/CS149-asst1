# 实验报告

## 程序 1

生成 view 1 时加速比与线程数的关系如下：

| 线程数 | 加速比 |
| ------ | ------ |
| 2      | 2.04   |
| 3      | 1.69   |
| 4      | 2.54   |
| 5      | 2.57   |
| 6      | 3.26   |
| 7      | 3.55   |
| 8      | 4.11   |

生成 view 2 时加速比与线程数的关系如下：

| 线程数 | 加速比 |
| ------ | ------ |
| 2      | 1.75   |
| 3      | 2.25   |
| 4      | 2.67   |
| 5      | 3.14   |
| 6      | 3.53   |
| 7      | 4.03   |
| 8      | 4.39   |

可以看到，生成 view 1 时加速比随线程数的增加并非在线性增长，在线程数由 2 增加到 3 时，甚至出现了下降。而生成 view 2 时，并未出现这种情况，加速比基本上随线程数的增加线性增长。猜测其原因在于**各线程工作量分配不均**。

通过在 `workerThreadStart()` 的开头和结尾插入计时代码来测量每个线程完成其工作的时间，得到如下结果：

<img src="https://cdn.jsdelivr.net/gh/BienBoy/images/images/2024%2F05%2F25%2F23-13-30-%E6%AF%8F%E4%B8%AA%E7%BA%BF%E7%A8%8B%E5%AE%8C%E6%88%90%E5%85%B6%E5%B7%A5%E4%BD%9C%E7%9A%84%E6%97%B6%E9%97%B4.png" alt="每个线程完成其工作的时间" style="zoom: 80%; float: left;" />

可以看到线程 1 计算所花费的时间远高于线程 0 和线程 2。证实了之前的猜测。联系题目描述：图像中的每个像素对应于复平面中的一个值，每个像素的亮度与确定该值是否包含在 Mandelbrot 中的计算成本成正比，观察生成的图片可以发现，中间部分较亮，计算成本高，而线程 1 恰好分配到中间这部分，所以耗时较多。

改进线程的工作分配：不再为线程连续分配工作，而是一行一行依次循环分配给各线程。

<img src="https://cdn.jsdelivr.net/gh/BienBoy/images/images/2024%2F05%2F25%2F23-26-18-%E6%9B%B4%E6%94%B9%E5%B7%A5%E4%BD%9C%E5%88%86%E9%85%8D%E6%96%B9%E5%BC%8F%E5%90%8E%E6%AF%8F%E4%B8%AA%E7%BA%BF%E7%A8%8B%E5%AE%8C%E6%88%90%E5%85%B6%E5%B7%A5%E4%BD%9C%E7%9A%84%E6%97%B6%E9%97%B4.png" alt="更改工作分配方式后每个线程完成其工作的时间" style="zoom:80%; float: left;" />

**最终的 8 线程加速比为 7.44。**

使用 16 个线程运行改进代码，加速比为 6.90。加速比不如 8 线程，这是因为程序所能达到的最大加速比收到程序中串行代码所占比例的限制：$最大加速比=\frac{1}{1-P}，其中 P 为并行部分所占比例$。当加速比趋于极限时，继续增大线程数反而会因为线程管理开销增大而导致性能下降。

## 程序 2

`./myexp -s 3` 结果如下：

<img src="https://cdn.jsdelivr.net/gh/BienBoy/images/images/2024%2F05%2F26%2F13-23-24-myexp%20-s%203.png" alt="myexp -s 3" style="zoom:80%; float: left;" />

`./myexp -s 10000`，且更改 VECTOR_WIDTH 的结果如下：

<div style="display: flex;flex-wrap: wrap;justify-content: space-between">
    <figure style="width:50%;">
        <img src="https://cdn.jsdelivr.net/gh/BienBoy/images/images/2024%2F05%2F26%2F14-35-37-2%20-%20myexp%20-s%2010000.png" alt="2 & myexp -s 10000"/>
        <figcaption>VECTOR_WIDTH = 2</figcaption>
    </figure>
    <figure style="width:50%;">
        <img src="https://cdn.jsdelivr.net/gh/BienBoy/images/images/2024%2F05%2F26%2F14-36-01-4%20-%20myexp%20-s%2010000.png" alt="4 & myexp -s 10000"/>
        <figcaption>VECTOR_WIDTH = 4</figcaption>
    </figure>
    <figure style="width:50%;margin-top: 20px;">
        <img src="https://cdn.jsdelivr.net/gh/BienBoy/images/images/2024%2F05%2F26%2F14-36-24-8%20-%20myexp%20-s%2010000.png" alt="8 & myexp -s 10000"/>
        <figcaption>VECTOR_WIDTH = 8</figcaption>
    </figure>
    <figure style="width:50%;margin-top: 20px;">
        <img src="https://cdn.jsdelivr.net/gh/BienBoy/images/images/2024%2F05%2F26%2F14-36-40-16%20-%20myexp%20-s%2010000.png" alt="16 & myexp -s 10000"/>
        <figcaption>VECTOR_WIDTH = 16</figcaption>
    </figure>
</div>

可以看到随着`VECTOR_WIDTH`的增大，向量利用率（Vector Utilization）在减小。`VECTOR_WIDTH`增大时，一条指令运行时会含有空操作的可能性也会增大。

`arraySumSerial`的时间复杂度为 $O(N)$，向量化后的实现`arraySumVector`时间复杂度为 $O((N / VECTOR\_WIDTH + log2(VECTOR\_WIDTH)))$。

## 程序 3

### 第 1 部分

期望的最大加速比是 8（因为 ISPC 编译器当前配置为发出 8 宽度的 AVX2 向量指令），实际加速比为 5.15。可能是 8 个通道间计算量不均衡导致了性能降低。

<img src="https://cdn.jsdelivr.net/gh/BienBoy/images/images/2024%2F05%2F26%2F14-35-14-ISPC.png" alt="ISPC" style="zoom:80%;float:left;" />

### 第 2 部分

启动 2 个 ISPC 任务后的加速比为 10.11，大约是未启动任务前的两倍。

<img src="https://cdn.jsdelivr.net/gh/BienBoy/images/images/2024%2F05%2F26%2F14-42-07-ISPC%20with%20tasks.png" alt="ISPC with tasks" style="zoom:80%;float:left;" />

我的电脑 CPU 型号为`Intel(R) Core(TM) i7-10510U CPU @ 1.80GHz   2.30 GHz`，有 4 个核，支持 8 个超线程，通过实验发现，任务数为 16 时，性能最高，加速比超过了 32。

<img src="https://cdn.jsdelivr.net/gh/BienBoy/images/images/2024%2F05%2F26%2F15-00-50-ISPC%20with%2016%20tasks.png" alt="ISPC with 16 tasks" style="zoom:80%; float:left;" />

个人理解：线程抽象和 ISPC 任务抽象的区别类似命令式和声明式。启动 10000 个线程，会实际创建这 10000 个线程运行，而启动 10000 个任务，编译器会根据硬件情况等创建合适数量的线程，这 10000 个任务组成任务池，每个线程从任务池中取出任务完成。（参考了 [Slide View : Parallel Computer Architecture and Programming](http://15418.courses.cs.cmu.edu/spring2016/lecture/progabstractions/slide_007) 中的回答）

## 程序 4

<img src="https://cdn.jsdelivr.net/gh/BienBoy/images/images/2024%2F05%2F26%2F15-35-23-sqrt.png" alt="sqrt" style="zoom:80%; float:left;"/>

ISPC 无任务时加速比为 4.33，有任务时加速比为 32.68。SIMD 并行化带来的加速比是 4.33，多核心并行化带来的加速比为 7.54，接近 8。

将 `values`的值均设为`2.999999f`时，加速比最大，无任务时加速比为 6.98，有任务时加速比为 55.21，提高了 SIMD 加速比，也提高了多核加速比（更加接近 8）。将 `values`的值均设为`2.999999f`会使得各通道及各任务的工作量都很均衡，同时计算量较大（增大了并行部分占比），因此增大了加速比。

<img src="https://cdn.jsdelivr.net/gh/BienBoy/images/images/2024%2F05%2F26%2F16-17-24-sqrt%20max%20speedup.png" alt="sqrt max speedup" style="zoom:80%;float:left;" />

将 `values`的按照每 8 个元素中有一个为`2.999999f`，其余为`1.f`时，加速比最小，无任务时加速比为 0.92，有任务时加速比为 7.11。因为编译时使用`--target=avx2`选项来生成 8 宽度的 SIMD 指令，这样设置 `values`可以使得 SIMD 8 个通道工作量不均衡，向量化版本和串行版本都主要在计算`values[i] = 2.999999f`时的结果，所以加速比小。

<img src="https://cdn.jsdelivr.net/gh/BienBoy/images/images/2024%2F05%2F26%2F16-17-21-sqrt%20min%20speedup.png" alt="sqrt min speedup" style="zoom:80%;float:left;" />

仅考虑`N`能被 8 整除的情况，使用 AVX2 内置函数实现`sqrt`~~（内置函数可以直接计算sqrt）~~，类似程序 2，结果如下：

<img src="https://cdn.jsdelivr.net/gh/BienBoy/images/images/2024%2F05%2F26%2F21-10-00-AVX2.png" alt="AVX2" style="zoom:80%; float: left;"/>

## 程序 5

<img src="https://cdn.jsdelivr.net/gh/BienBoy/images/images/2024%2F05%2F26%2F16-19-18-saxpy.png" alt="saxpy" style="zoom:80%;float:left;" />

有任务的 ISPC 的加速比为 1.42，性能提升较少。`saxpy`属于I/O密集型程序，相对于计算，其用于读写内存的时间要更多，性能瓶颈在于内存带宽的限制，无法通过重写代码大幅改进性能。

`main.cpp`中计算消耗的总内存带宽为 `TOTAL_BYTES = 4 * N * sizeof(float);`，乘以 4 是因为存在缓存，要更改`result`的值，需要先将其读入缓存，更改之后再写回内存，涉及到两次 I/O。

## 程序 6

根据`main.cpp`注释中的代码可以生成`data.dat`。

可以发现，`kMeansThread`中主要使用三个函数：`computeAssignments`、`computeCentroids`和`computeCost`。分别对其计时，可以发现`computeAssignments`耗时最长，应将其并行化以提高性能。

`computeAssignments`的功能是将数据点划分到距离最近的聚类中，数据点之间没有依赖，可以并行化。进行数据划分时，应该将计算每个点所属聚类作为最小任务单元（对应循环`for (int m = 0; m < args->M; m++)`），这样比按照聚类中心划分工作要更加均衡。

加速前，代码运行时间为：

<img src="https://cdn.jsdelivr.net/gh/BienBoy/images/images/2024%2F05%2F26%2F19-11-04-K-Means.png" alt="K-Means" style="zoom:80%;float: left;" />

加速后，代码运行时间为：

<img src="https://cdn.jsdelivr.net/gh/BienBoy/images/images/2024%2F05%2F26%2F19-11-21-Multithreading%20K-Means.png" alt="Multithreading K-Means" style="zoom:80%;float: left;" />

加速比为：$18716.043 / 7125.182=2.63$
