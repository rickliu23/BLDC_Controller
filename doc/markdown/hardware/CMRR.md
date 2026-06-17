+ CMRR：共模抑制比

# 1 如何根据共模抑制比算误差
+ 以INA199A1DCKR为例
	+ 共模抑制比：100db
	+ 同相输入端10V
	+ 反相输入端9.9V
	+ 固定增益50
## 1.1 算差模和共模
+ 差模：
$$V_{diff} = V_{+} - V_{-} = 10 - 9.9 = 0.1 \text{ V} = 100 \text{ mV}$$
共模：
$$V_{cm} = \frac{V_{+} + V_{-}}{2} = \frac{10 + 9.9}{2} = 9.95 \text{ V}$$
## 1.2 算共模泄漏误差

$$V_{error} = \frac{V_{cm}}{10^{CMRR/20}} = \frac{9.95}{10^{100/20}} = \frac{9.95}{10^{5}} = \frac{9.95}{100000} = 99.5 \text{ µV}$$

## 1.3 算输出端各分量

|分量|计算|结果|
|:--|:--|:--|
|理想差模输出|0.1 V×50|5 V|
|共模泄漏输出|99.5 µV×50|4.975 mV|
|实际输出|5 V+4.975 mV|≈5.005 V|


## 1.4 最终误差评估
$$\text{误差比例} = \frac{4.975 \text{ mV}}{5 \text{ V}} \times 100\% = 0.0995\% \approx 0.1\%$$


# 2 根据需求选型
## 2.1 需求
|参数| 数值           |
|:--|:--|
|最大共模电压 Vcm​| 12 V         |
|最小差模信号 Vdiff​| 0.05 V=50 mV |
|允许误差| 1%           |
|差模增益 Adiff​| 50           
## 2.2 快速公式

$$CMRR_{required} = 20 \cdot \log_{10}\left(\frac{V_{cm}}{V_{diff} \times \text{允许误差}}\right)$$
+ 带入需求中所列参数
$$CMRR = 20 \cdot \log_{10}\left(\frac{12}{0.05 \times 0.01}\right) = 20 \cdot \log_{10}(24000) \approx 88 \text{ dB}$$

## 2.3 计算
### 2.3.1 允许的共模泄漏（输入端）

$$\text{允许误差电压} = V_{diff} \times 1\% = 50 \text{ mV} \times 0.01 = 0.5 \text{ mV} = 500 \text{ µV}$$
### 2.3.2 反推所需 CMRR

$$CMRR_{required} = 20 \cdot \log_{10}\left(\frac{V_{cm}}{V_{error}}\right) = 20 \cdot \log_{10}\left(\frac{12 \text{ V}}{0.5 \text{ mV}}\right)$$

$$= 20 \cdot \log_{10}\left(\frac{12}{0.0005}\right) = 20 \cdot \log_{10}(24000)$$

$$= 20 \times 4.38 = 87.6 \text{ dB}$$
### 2.3.3 验证输出
|分量|计算|结果|
|:--|:--|:--|
|理想差模输出|50 mV×50|2.5 V|
|共模泄漏输出|0.5 mV×50|25 mV|
|实际输出|2.5 V+25 mV|2.525 V|
|相对误差|25 mV/2.5 V|1% ✅|
