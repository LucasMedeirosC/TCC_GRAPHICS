import numpy as np
import matplotlib.pyplot as plt
import os

# Criar pasta de imagens se não existir
if not os.path.exists('imagens_metodologia'):
    os.makedirs('imagens_metodologia')

# parâmetros do sinal
f = 60                # frequência da rede
fs = 10000            # frequência de amostragem
t = np.linspace(0, 3/f, int(3*fs/f))  # 3 ciclos

# sinal senoidal
v = np.sin(2*np.pi*f*t)

# amostras por ciclo
N = int(fs/f)
halfN = int(N/2)

def plot_window(start, size, filename):
    
    plt.figure()

    plt.plot(t, v)

    # região da janela
    end = start + size
    plt.axvspan(t[start], t[end], alpha=0.3)

    plt.xlabel("Tempo (s)", fontsize=13, fontweight="bold")
    plt.ylabel("Tensão (pu)", fontsize=13, fontweight="bold")
    plt.xticks(fontsize=11, fontweight="bold")
    plt.yticks(fontsize=11, fontweight="bold")
    plt.grid()

    plt.savefig(filename, dpi=300, bbox_inches='tight')
    plt.close()


# -------------------------
# 1 ciclo completo
# -------------------------

plot_window(
    start=167,
    size=N,
    filename="imagens_metodologia/janela_ciclo_completo.png"
)

# -------------------------
# meio ciclo
# -------------------------

plot_window(
    start=167,
    size=halfN,
    filename="imagens_metodologia/janela_meio_ciclo.png"
)

# -------------------------
# janela móvel A
# -------------------------

plot_window(
    start=167,
    size=N,
    filename="imagens_metodologia/janela_movel_A.png"
)

# -------------------------
# janela móvel B
# -------------------------

plot_window(
    start=187,
    size=N,
    filename="imagens_metodologia/janela_movel_B.png"
)

# -------------------------
# janela móvel C
# -------------------------

plot_window(
    start=207,
    size=N,
    filename="imagens_metodologia/janela_movel_C.png"
)

print("Imagens geradas com sucesso.")