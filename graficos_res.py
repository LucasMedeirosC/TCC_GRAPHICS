import pandas as pd
import matplotlib.pyplot as plt
import os
import numpy as np

# ================================
# Criar pasta de saída
# ================================
pasta = "imagens_resultados"
os.makedirs(pasta, exist_ok=True)

# ================================
# Calcular Limite de Detecção
# ================================
amplitude = 311.0  # Valor pico da tensão (V)
rms_nominal = amplitude / np.sqrt(2)  # RMS nominal (220V)
limite = 0.9 * rms_nominal  # Limite de 90%

# ================================
# Ler CSV
# ================================
df = pd.read_csv("output/dados_rms.csv")

tempo = df["tempo"]
onda = df["tensao"]

# Picos
pico_meio = df["pico_meio"]
pico_ciclo = df["pico_ciclo"]

# RMS
rms_bloco = df["rms_bloco"]
rms_pico_bloco = pico_ciclo   # conversão de pico → RMS
rms_movel_meio = df["rms_movel_meio"]
rms_movel_ciclo = df["rms_movel"]

# Flags
flag_real = df["sag_real"]
flag_movel = df["flag_movel"]
flag_meio = df["flag_meio_movel"]
flag_bloco = df["flag_bloco"]
flag_pico_meio = df["flag_pico_meio"]
flag_pico_ciclo = df["flag_pico_ciclo"]

# ================================
# Determinar intervalos de SAG
# ================================
sag_indices = np.where(flag_real == 1)[0]
sag_intervals = []
if len(sag_indices) > 0:
    start = sag_indices[0]
    for i in range(1, len(sag_indices)):
        if sag_indices[i] - sag_indices[i-1] > 1:
            sag_intervals.append((tempo.iloc[start], tempo.iloc[sag_indices[i-1]]))
            start = sag_indices[i]
    sag_intervals.append((tempo.iloc[start], tempo.iloc[sag_indices[-1]]))

# Determinar primeiro instante de detecção
def t_detect(flag, tempo):
    idx = flag.idxmax()
    return tempo[idx] if flag[idx] == 1 else None


# ===============================================
# 1) Onda Simples com Marcação de SAG
# ===============================================
plt.figure(figsize=(14,6))
plt.plot(tempo, onda, label="Tensão (Onda Senoidal)", linewidth=3)

# Marcar regiões de SAG
for t_start, t_end in sag_intervals:
    plt.axvspan(t_start, t_end, alpha=0.2, color="red", label="Região de SAG" if t_start == sag_intervals[0][0] else "")
# Título: "Onda de Tensão com Marcação de SAG"
plt.xlabel("Tempo (s)", fontsize=13, fontweight="bold")
plt.ylabel("Amplitude (V)", fontsize=13, fontweight="bold")
plt.xticks(fontsize=11, fontweight="bold")
plt.yticks(fontsize=11, fontweight="bold")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig(f"{pasta}/01_onda_sag.png", dpi=300)
plt.close()

# ===============================================
# 2) Onda + Comparação dos dois métodos de pico
# ===============================================
plt.figure(figsize=(14,6))
plt.plot(tempo, onda, label="Tensão (Onda Senoidal)", linewidth=3)
plt.plot(tempo, pico_meio, label="Pico Meio Ciclo", linestyle="--", linewidth=2)
plt.plot(tempo, pico_ciclo, label="Pico Ciclo Completo", linestyle="--", linewidth=2)
plt.axhline(y=limite, color="red", linestyle="--", linewidth=2.5, label=f"Limite 90% RMS ({limite:.2f}V)")

# Marcar regiões de SAG
for t_start, t_end in sag_intervals:
    plt.axvspan(t_start, t_end, alpha=0.2, color="red", label="Região de SAG" if t_start == sag_intervals[0][0] else "")
# Título: "Comparação de Métodos de Pico"
plt.xlabel("Tempo (s)", fontsize=13, fontweight="bold")
plt.ylabel("Amplitude", fontsize=13, fontweight="bold")
plt.xticks(fontsize=11, fontweight="bold")
plt.yticks(fontsize=11, fontweight="bold")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig(f"{pasta}/02_comparacao_picos.png", dpi=300)
plt.close()

# ===============================================
# 3) Comparativo RMS bloco vs RMS pico-bloco
# ===============================================
plt.figure(figsize=(14,6))
plt.plot(tempo, onda, label="Tensão", linewidth=3)
plt.plot(tempo, rms_bloco, label="RMS Bloco", linestyle="--", linewidth=2)
plt.plot(tempo, rms_pico_bloco, label="RMS Pico Bloco", linestyle="--", linewidth=2)
plt.axhline(y=limite, color="red", linestyle="--", linewidth=2.5, label=f"Limite 90% RMS ({limite:.2f}V)")

# Marcar regiões de SAG
for t_start, t_end in sag_intervals:
    plt.axvspan(t_start, t_end, alpha=0.2, color="red", label="Região de SAG" if t_start == sag_intervals[0][0] else "")
# Título: "RMS Bloco VS RMS Pico Bloco"
plt.xlabel("Tempo (s)", fontsize=13, fontweight="bold")
plt.ylabel("Valor", fontsize=13, fontweight="bold")
plt.xticks(fontsize=11, fontweight="bold")
plt.yticks(fontsize=11, fontweight="bold")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig(f"{pasta}/03_rms_bloco_vs_pico_bloco.png", dpi=300)
plt.close()

# ===============================================
# 4) RMS Móvel Meio Ciclo x RMS Móvel Ciclo Completo
# ===============================================
plt.figure(figsize=(14,6))
plt.plot(tempo, onda, label="Tensão", linewidth=3)
plt.plot(tempo, rms_movel_meio, label="RMS Móvel Meio Ciclo", linestyle="--", linewidth=2)
plt.plot(tempo, rms_movel_ciclo, label="RMS Móvel Ciclo Completo", linestyle="--", linewidth=2)
plt.axhline(y=limite, color="red", linestyle="--", linewidth=2.5, label=f"Limite 90% RMS ({limite:.2f}V)")

# Marcar regiões de SAG
for t_start, t_end in sag_intervals:
    plt.axvspan(t_start, t_end, alpha=0.2, color="red", label="Região de SAG" if t_start == sag_intervals[0][0] else "")
# Título: "RMS Móvel: Meio Ciclo vs Ciclo Completo"
plt.xlabel("Tempo (s)", fontsize=13, fontweight="bold")
plt.ylabel("Valor", fontsize=13, fontweight="bold")
plt.xticks(fontsize=11, fontweight="bold")
plt.yticks(fontsize=11, fontweight="bold")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig(f"{pasta}/04_rms_movel_comparativo.png", dpi=300)
plt.close()

# ===============================================
# 5) Flags em subplots separados + cores + legenda única
#     ORDENADOS do mais rápido ao mais lento
# ===============================================

import matplotlib.pyplot as plt

def tempo_detect(flag, tempo):
    """Retorna o primeiro tempo onde flag == 1. Se não detectar, retorna um valor grande."""
    idx = flag.to_numpy().nonzero()[0]
    return tempo[idx[0]] if len(idx) > 0 else 999

# Lista de flags com nome e cor
lista_flags = [
    ("Sag Real", flag_real, "red"),
    ("Pico Meio Ciclo", flag_pico_meio, "blue"),
    ("Pico Ciclo Completo", flag_pico_ciclo, "green"),
    ("RMS Móvel Meio Ciclo", flag_meio, "orange"),
    ("RMS Móvel Ciclo Completo", flag_movel, "purple"),
    ("RMS Bloco", flag_bloco, "brown"),
]

# Ordenar do mais rápido ao mais lento (exceto Sag Real que vai sempre em primeiro)
metodos = lista_flags[1:]  # sem a real
metodos_ordenados = sorted(metodos, key=lambda x: tempo_detect(x[1], tempo))

# Inserir Sag Real no topo
lista_ordenada = [lista_flags[0]] + metodos_ordenados

# Criar subplots
fig, axes = plt.subplots(len(lista_ordenada), 1, figsize=(14, 18), sharex=True)

handles = []
labels = []

for ax, (nome, flag, cor) in zip(axes, lista_ordenada):
    h, = ax.plot(tempo, flag, color=cor, linewidth=2.5)
    ax.set_ylabel("Flag", fontsize=11, fontweight="bold")
    ax.set_title(nome + f" (t={tempo_detect(flag, tempo):.4f}s)", fontsize=12, fontweight="bold")
    ax.tick_params(axis='both', which='major', labelsize=11)
    for label in ax.get_xticklabels():
        label.set_fontweight('bold')
    for label in ax.get_yticklabels():
        label.set_fontweight('bold')
    ax.grid(True)

    handles.append(h)
    labels.append(nome)

axes[-1].set_xlabel("Tempo (s)", fontsize=13, fontweight="bold")

# Título: "Flags de Detecção Organizadas do Mais Rápido ao Mais Lento"
plt.tight_layout()

# Legenda única
legend = fig.legend(handles, labels, loc="lower center", ncol=3, fontsize=12, bbox_to_anchor=(0.5, -0.05))
for text in legend.get_texts():
    text.set_fontweight('bold')

plt.savefig(f"{pasta}/05_flags_subplots.png", dpi=300)
plt.close()

# ===============================================
# 6) Erro de Detecção (Delay)
# ===============================================
# Encontrar tempo real do SAG
sag_real_idx = np.where(flag_real.values == 1)[0]
tempo_sag_real = tempo.iloc[sag_real_idx[0]] if len(sag_real_idx) > 0 else 0

# Criar lista com (nome, tempo_deteccao, cor)
metodos_deteccao = [
    ("RMS Móvel\n1 Ciclo", flag_movel, "purple"),
    ("RMS Móvel\nMeio Ciclo", flag_meio, "orange"),
    ("RMS Bloco", flag_bloco, "brown"),
    ("Pico Meio\nCiclo", flag_pico_meio, "blue"),
    ("Pico Ciclo\nCompleto", flag_pico_ciclo, "green"),
]

# Calcular tempo de detecção para cada método
tempos_com_info = []
for nome, flag, cor in metodos_deteccao:
    idx = np.where(flag.values == 1)[0]
    tempo_det = tempo.iloc[idx[0]] if len(idx) > 0 else np.nan
    tempos_com_info.append((nome, tempo_det, cor))

# Ordenar por tempo de detecção (mais rápido à esquerda, mais lento à direita)
tempos_com_info_sorted = sorted(tempos_com_info, key=lambda x: x[1] if not np.isnan(x[1]) else 999, reverse=False)

metodos_nomes_ordered = [item[0] for item in tempos_com_info_sorted]
tempos_deteccao_ordered = [item[1] for item in tempos_com_info_sorted]
cores_ordered = [item[2] for item in tempos_com_info_sorted]

# Calcular erro de detecção
erros_deteccao_ordered = [td - tempo_sag_real if not np.isnan(td) else np.nan for td in tempos_deteccao_ordered]

fig, ax = plt.subplots(figsize=(14, 6))
bars = ax.bar(metodos_nomes_ordered, erros_deteccao_ordered, color=cores_ordered, alpha=0.7, edgecolor="black", linewidth=2)
ax.axhline(y=0, color="black", linestyle="-", linewidth=2.5)
ax.set_ylabel("Delay (s)", fontsize=13, fontweight="bold")
ax.tick_params(axis='both', which='major', labelsize=11, colors='black')
for label in ax.get_xticklabels():
    label.set_fontweight('bold')
for label in ax.get_yticklabels():
    label.set_fontweight('bold')
# Título: "Erro de Detecção - Delay relativo ao SAG Real (Mais Rápido → Mais Lento)"
ax.grid(True, alpha=0.3, axis="y")

# Adicionar valores nas barras
for bar, erro in zip(bars, erros_deteccao_ordered):
    if not np.isnan(erro):
        height = bar.get_height()
        va = 'bottom' if height >= 0 else 'top'
        y_pos = height if height >= 0 else height
        ax.text(bar.get_x() + bar.get_width()/2., y_pos,
                f'{erro:.4f}s',
                ha='center', va=va, fontweight='bold', fontsize=10)

plt.tight_layout()
plt.savefig(f"{pasta}/06_erro_deteccao.png", dpi=300)
plt.close()

print("Imagens geradas em: imagens_resultados/")