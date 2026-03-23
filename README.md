# TCC_GRAPHICS

Projeto para simulação e análise de métodos de detecção de afundamento de tensão (SAG), com geração de dados em C e visualização dos resultados em Python.

## Objetivo

Comparar o desempenho de métodos de detecção de SAG em sinais de tensão de 60 Hz, avaliando principalmente:

- Tempo de detecção (delay);
- Comportamento de cada método durante o evento;
- Diferença entre abordagens por RMS e por pico.

## Estrutura do projeto

```text
TCC_GRAPHICS/
├── teste.c                 # Simulação principal e geração do CSV
├── teste3.c                # Teste de PLL (versão standalone)
├── PLL.c                   # Implementação da PLL
├── PLL.h                   # Interface da PLL
├── graficos_met.py         # Gera figuras da metodologia
├── graficos_res.py         # Gera figuras de resultados a partir do CSV
├── output/
│   └── dados_rms.csv       # Dados de saída da simulação
├── imagens_metodologia/    # Figuras explicativas de janelas
└── imagens_resultados/     # Figuras finais de comparação
```

## Dependências

### C

- Compilador C com suporte a C99+ (ex.: GCC/MinGW).

### Python

- Python 3.9+;
- Bibliotecas:
	- `numpy`
	- `pandas`
	- `matplotlib`

Instalação rápida:

```bash
pip install numpy pandas matplotlib
```

## Como executar

### 1) Gerar os dados (`dados_rms.csv`)

No diretório do projeto:

```bash
gcc teste.c -o output/teste.exe -lm
./output/teste.exe
```

No Windows (PowerShell), também pode executar com:

```powershell
.\output\teste.exe
```

Isso gera (ou atualiza) o arquivo `output/dados_rms.csv`.

### 2) Gerar figuras da metodologia

```bash
python graficos_met.py
```

Saída em `imagens_metodologia/`:

- `janela_ciclo_completo.png`
- `janela_meio_ciclo.png`
- `janela_movel_A.png`
- `janela_movel_B.png`
- `janela_movel_C.png`

### 3) Gerar figuras de resultados

```bash
python graficos_res.py
```

Saída em `imagens_resultados/`:

- `01_onda_sag.png`
- `02_comparacao_picos.png`
- `03_rms_bloco_vs_pico_bloco.png`
- `04_rms_movel_comparativo.png`
- `05_flags_subplots.png`
- `06_erro_deteccao.png`

## Métodos comparados

Os métodos implementados na simulação (`teste.c`) são:

- RMS móvel (1 ciclo);
- RMS móvel (meio ciclo);
- RMS em bloco;
- Pico por meio ciclo (convertido para RMS);
- Pico por ciclo completo (convertido para RMS).

O SAG real é inserido no intervalo aproximado de `0.10 s` a `0.16 s` para comparação com as flags de detecção.

## Observações

- O script `graficos_res.py` espera encontrar `output/dados_rms.csv` com as colunas geradas por `teste.c`.
- Se a compilação gerar o executável fora de `output/`, mova-o ou ajuste o comando de execução.
- Os arquivos de imagem são sobrescritos a cada nova execução dos scripts Python.