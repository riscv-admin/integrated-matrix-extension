
# Integrated Matrix Extension

* Task Group Info
  * [Group page](https://lists.riscv.org/g/tech-integrated-matrix-extension)
  * [Mailing list](https://lists.riscv.org/g/tech-integrated-matrix-extension/topics)
* Meeting Details
  * [Official RISC-V Technical Calendar](https://tech.riscv.org/calendar/)
  * Date: Odd weeks on Mon.
  * Time: 08:00 (PDT), 11:00 (EDT), 12:00 (BRT), 16:00 (CET), 23:00 (CST/TST), 24:00 (JST).
  * [Zoom link](https://zoom.us/j/93397157767?pwd=UE0vbWJEU0dFSXR4dlp5NGZjaUJJdz09)
  * Meeting ID: 933 9715 7767
  * Passcode: 051351
* Disclaimer
  * [Slides](https://docs.google.com/presentation/d/1LNhpuNwU54TgwGfcl-Fgf4HUFxCxh0AztPaeqMuRQRw/edit#slide=id.p1)
  * [Video](https://drive.google.com/file/d/1N_LM_XLLJ0YqadTF2VaAVB7I7QXgapc0/view?usp=sharing) 
* Acting Chairs
  * Guido Araujo  (Acting Chair, UNICAMP)
  * Jose Moreira  (Acting Vice-chair, IBM)
* Meetings
  * Next meeting: [2024/08/09](https://github.com/riscv-admin/integrated-matrix-extension/wiki/IME-TG-Minutes#20240809)
  * Previous meeting: [2024/07/29](https://github.com/riscv-admin/integrated-matrix-extension/wiki/IME-TG-Minutes#20240729)  
  * [All Minutes](https://github.com/riscv-admin/integrated-matrix-extension/wiki/IME-TG-Minutes)
  * [All Slides](https://github.com/riscv-admin/integrated-matrix-extension/tree/main/Meetings)
  * Videos
* [Roadmap](https://docs.google.com/spreadsheets/d/170fX-avrKYMDfF3kjlzQ6lpqC0c4ytTd/edit?usp=sharing&ouid=112376118999052114595&rtpof=true&sd=true)
* [Working groups](https://docs.google.com/spreadsheets/d/1JvsnjbWNu0991DGnbOnMhwTgKnAZUJlsPKbz9vBXNow/edit#gid=0)
* [Articles](https://github.com/riscv-admin/integrated-matrix-extension/tree/main/Articles)
* Presentations
  * [Architectures](https://github.com/riscv-admin/integrated-matrix-extension/tree/main/Presentations/Architectures)
  * [Memory Analysis](https://github.com/riscv-admin/integrated-matrix-extension/tree/main/Presentations/Memory%20Analysis)
  * [uArch](https://github.com/riscv-admin/integrated-matrix-extension/tree/main/Presentations/uArch)
  * [Workloads](https://github.com/riscv-admin/integrated-matrix-extension/tree/main/Presentations/Workloads)
* Notebooks and code
  * [[Andes and Unicamp] Computational intensity notebook](https://colab.research.google.com/drive/1TGw2BgNmbv2rLj0cicEssIBN_9Kr-2HS?usp=sharing)
  * [[Unicamp] Burst analysis notebook](https://colab.research.google.com/drive/1sJk-rhXR94mZpjDoKmHjjcnjxcZwsnoV?usp=sharing)
  * [[Andes] Cache and DRAM evaluation tool](https://github.com/CN-Ke/IME_Evaluation/blob/main/compute_locality.py)
  * [[Unicamp] Revisiting CI under a datatype glass](https://colab.research.google.com/drive/1TGw2BgNmbv2rLj0cicEssIBN_9Kr-2HS?pli=1&usp=drive_fs)
* Workloads and Benchmarking
  * [[Alibaba] Analysis of matrix and vector instructions of ML Models](https://github.com/T-head-Semi/riscv-matrix-extension-spec/blob/master/doc/slides/AME_workload_analysis_20240412.pdf)
  * [[Unicamp] Analysis of vector and matrix transfers in Polybench (HPC benchmark)](https://docs.google.com/spreadsheets/d/1FmBtJt__I1hj4LtRNhwMqrIsKPjSK40DEUBpWAi2QAQ/edit#gid=0)
  * [Unicamp] ConvBench
      * [Repository](https://github.com/LucasFernando-aes/ConvBench)
      * [Slides](https://docs.google.com/presentation/d/1WCXeOk6pKTYOVMLlb8tDQn18MKf7jVpEqTrZZHRs5u8/edit?usp=sharing)
      * [Video](https://drive.google.com/file/d/177l3FYafUxJo7HYsiRobBlvAcOccPi7g/view?usp=sharing)
   * [Eldorado and Unicamp] IBM POWER10 MMA Vector-Matrix Interaction
      * Thanks to Jose Moreira!
      * [Description](https://docs.google.com/document/d/18HNujobGMmEH6jhbmQu5rA1mwl8Wo91m?rtpof=true&usp=drive_fs)
      * [Data](https://docs.google.com/spreadsheets/d/18KxYZu3Re2Z2f8C2Pttf_6PvO9PxnlOq?rtpof=true&usp=drive_fs)
 * Market Analysis
    * [Companies Need On-Premise HPC – and for More Than AI, Too](https://www.nextplatform.com/2024/05/20/companies-need-on-premise-hpc-and-for-more-than-ai-too/)
    * [Hyperion Research Analysis](https://hyperionresearch.com/wp-content/uploads/2024/05/Hyperion-Research-Pre-ISC24-Presentations-May-2024.pdf)
  * Mathematical Analysis
    * [Lemmata](https://drive.google.com/open?id=1okPUNbpweYShNuifNVnARrKAImTp4mTJ&usp=drive_fs)
      * k = 1 maximizes computational intensity under architected state constraints
      * Computational intensity is maximized for m = n
    * [Corollaries](https://drive.google.com/open?id=1ookgA_hf4YufXP_23DTMZHQDq2aAALDd&usp=drive_fs)
      * When vector registers are used as accumulators maximum computational intensity is  ½ * (sqrt(RV + 1) – 1). 
      * When vector registers are used only as operands and accumulators are newly designed states, computational intensity is always larger and is given by RV/4.

     
  
  


