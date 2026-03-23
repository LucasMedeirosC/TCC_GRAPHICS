#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#define FS 10000.0f
#define FREQ 60.0f
#define N 167
#define N_MEIO (N/2)
#define DURACAO 0.25f

#define PI 3.14159265358979323846f

float period[N] = {0};
float period_meio[N_MEIO] = {0};

/* ========================= RMS MOVEL 1 CICLO ========================= */
float calcular_rms_movel(float amostra, float *soma, int indice)
{
    float novo = amostra * amostra;
    float antigo = period[indice] * period[indice];

    *soma -= antigo;
    *soma += novo;

    return sqrtf(*soma / N);
}

/* ========================= RMS MOVEL MEIO CICLO ========================= */
float calcular_rms_movel_meio(float amostra, float *soma, int indice)
{
    float novo = amostra * amostra;
    float antigo = period_meio[indice] * period_meio[indice];

    *soma -= antigo;
    *soma += novo;

    return sqrtf(*soma / N_MEIO);
}

/* ========================= RMS BLOCO ========================= */
float calcular_rms_bloco(float amostra, float *soma,
                         int *contador, float rms_ant)
{
    *soma += amostra * amostra;
    (*contador)++;

    if(*contador >= N)
    {
        float rms = sqrtf(*soma / N);
        *soma = 0;
        *contador = 0;
        return rms;
    }

    return rms_ant;
}

/* ========================= MAIN ========================= */
int main()
{
    FILE *fp = fopen("dados_rms.csv", "w");
    if(!fp)
    {
        printf("Erro ao criar CSV\n");
        return 1;
    }

    fprintf(fp,"tempo,tensao,rms_movel,rms_movel_meio,rms_bloco,"
               "pico_meio,pico_ciclo,"
               "sag_real,flag_movel,flag_meio_movel,"
               "flag_bloco,flag_pico_meio,flag_pico_ciclo\n");

    float Ts = 1.0f / FS;
    float T_ciclo = 1.0f / FREQ;
    int total = (int)(DURACAO * FS);

    float soma_movel=0, soma_meio=0, soma_bloco=0;

    float rms_movel=0, rms_meio_movel=0, rms_bloco=0;
    float rms_pico_meio=0, rms_pico_ciclo=0;

    float pico_meio=0, pico_ciclo=0;

    int i=0, i_meio=0;
    int cont_bloco=0;
    int cont_meio=0, cont_ciclo=0;

    float amplitude=311.0f;
    float rms_nominal=amplitude/sqrtf(2.0f);
    float limite=0.9f*rms_nominal;

    float inicio_sag=0.10f;
    float fim_sag=0.16f;

    int flag_movel=0, flag_meio_movel=0;
    int flag_bloco=0, flag_pico_meio=0, flag_pico_ciclo=0;

    float t_det_movel=-1, t_det_meio_movel=-1;
    float t_det_bloco=-1, t_det_pico_meio=-1, t_det_pico_ciclo=-1;

    for(int k=0;k<total;k++)
    {
        float tempo=k*Ts;

        int sag_real=0;
        float amp=amplitude;

        if(tempo>=inicio_sag && tempo<=fim_sag)
        {
            amp*=0.5f;
            sag_real=1;
        }

        float amostra=amp*sinf(2*PI*FREQ*tempo);

        /* ===== RMS MOVEL 1 CICLO ===== */
        rms_movel=calcular_rms_movel(amostra,&soma_movel,i);

        if(k>N && rms_movel<limite)
        {
            if(!flag_movel) t_det_movel=tempo;
            flag_movel=1;
        }
        else flag_movel=0;

        period[i]=amostra;

        /* ===== RMS MOVEL MEIO CICLO ===== */
        rms_meio_movel=calcular_rms_movel_meio(amostra,&soma_meio,i_meio);

        if(k>N_MEIO && rms_meio_movel<limite)
        {
            if(!flag_meio_movel) t_det_meio_movel=tempo;
            flag_meio_movel=1;
        }
        else flag_meio_movel=0;

        period_meio[i_meio]=amostra;

        /* ===== RMS BLOCO ===== */
        rms_bloco=calcular_rms_bloco(amostra,&soma_bloco,&cont_bloco,rms_bloco);

        if(k>N && rms_bloco<limite)
        {
            if(!flag_bloco) t_det_bloco=tempo;
            flag_bloco=1;
        }
        else flag_bloco=0;

        /* ===== PICO MEIO ===== */
        float abs_val=fabsf(amostra);

        if(abs_val>pico_meio) pico_meio=abs_val;
        cont_meio++;

        if(cont_meio>=N_MEIO)
        {
            rms_pico_meio=pico_meio/sqrtf(2.0f);

            if(k>N && rms_pico_meio<limite)
            {
                if(!flag_pico_meio) t_det_pico_meio=tempo;
                flag_pico_meio=1;
            }
            else flag_pico_meio=0;

            pico_meio=0;
            cont_meio=0;
        }

        /* ===== PICO CICLO ===== */
        if(abs_val>pico_ciclo) pico_ciclo=abs_val;
        cont_ciclo++;

        if(cont_ciclo>=N)
        {
            rms_pico_ciclo=pico_ciclo/sqrtf(2.0f);

            if(k>N && rms_pico_ciclo<limite)
            {
                if(!flag_pico_ciclo) t_det_pico_ciclo=tempo;
                flag_pico_ciclo=1;
            }
            else flag_pico_ciclo=0;

            pico_ciclo=0;
            cont_ciclo=0;
        }

        fprintf(fp,"%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%d,%d,%d,%d,%d,%d\n",
                tempo,amostra,
                rms_movel,rms_meio_movel,rms_bloco,
                rms_pico_meio,rms_pico_ciclo,
                sag_real,
                flag_movel,flag_meio_movel,
                flag_bloco,flag_pico_meio,flag_pico_ciclo);

        i++; if(i>=N) i=0;
        i_meio++; if(i_meio>=N_MEIO) i_meio=0;
    }

    fclose(fp);

    printf("\n=========== RESULTADOS ===========\n");
    printf("Sag iniciou: %.6f s\n\n",inicio_sag);

    printf("RMS MOVEL 1 CICLO: %.6f s\n",(t_det_movel));
    printf("RMS MOVEL MEIO CICLO: %.6f s\n",(t_det_meio_movel));
    printf("RMS BLOCO: %.6f s\n",(t_det_bloco));
    printf("PICO MEIO CICLO: %.6f s\n",(t_det_pico_meio));
    printf("PICO CICLO: %.6f s\n",(t_det_pico_ciclo));

    printf("\nCSV gerado com sucesso.\n");

    return 0;
}