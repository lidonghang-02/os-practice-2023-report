# 控制流（Control Flow）和Trap
1. 控制流
- branch，jump
2. 异常控制流（ECP）
- exception
- interrupt
3. RISC-V把ECP统称为Trap
# RISC-V Trap处理中涉及的寄 存器
- Machine模式下的CSR列表
![](pic/pic1/Pasted%20image%2020230804135258.png)
- mtvec寄存器
![](pic/pic1/Pasted%20image%2020230804135517.png)
MODE = Direct
![](pic/pic1/Pasted%20image%2020230804135859.png)
MODE = Vectored
![](pic/pic1/Pasted%20image%2020230804140005.png)
- mepc寄存器，保存pc指令的地址用于推出trap时返回
![](pic/pic1/Pasted%20image%2020230804140350.png)
- mcause寄存器，存储异常或中断发生原因
![](pic/pic1/Pasted%20image%2020230804140707.png)
- mtval寄存器，配合mcause寄存器可以得道异常发送的详细信息
![](pic/pic1/Pasted%20image%2020230804140945.png)
- mstatus，描述状态信息
![](pic/pic1/Pasted%20image%2020230804141148.png)
没有UPP是因为当异常发生时，权限从低向高跳转，即`U->M`、`S->M`、`M->M`。所以对于M来说有3种情况，因此MPP用2位比特来表示；对于S来说有`U->S`、`S->S`两种情况，所以SPP用1位表示；但是对于U来说只有`U->U`一种情况，没必要用额外的比特来表示。
# RISC-V Trap处理流程
总体来说有4大步
![](pic/pic1/Pasted%20image%2020230804141951.png)
1. Trap初始化
![](pic/pic1/Pasted%20image%2020230804142025.png)
2. Trap的Top Half（硬件实现）
![](pic/pic1/Pasted%20image%2020230804142121.png)
3. Trap的Bottom Half（中断或异常的处理函数）
![](pic/pic1/Pasted%20image%2020230804144501.png)
4. 从Trap返回（恢复现场）
![](pic/pic1/Pasted%20image%2020230804144559.png)
