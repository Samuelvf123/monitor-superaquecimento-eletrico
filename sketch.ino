#include <OneWire.h>
#include <DallasTemperature.h>

/* ---------- TEMPERATURA (DS18B20) ---------- */
const int pino_sensor = 2;            /* DQ do DS18B20 no GPIO 2 */
const float lim_temp = 40.0;          /* limite de temperatura alta (C) */
OneWire oneWire(pino_sensor);
DallasTemperature sensor(&oneWire);
float prev_temp = 0.0;

/* ---------- CORRENTE (potenciometro simula o INA219) ---------- */
const int pino_pot = 34;              /* pino analogico do ESP32 */
const float lim_corrente = 1000.0;    /* limite de corrente alta (mA) */
float prev_corrente = 0.0;

/* ---------- controle ---------- */
int i_leitura = 0;                    /* conta as leituras */

/* calcula a variacao de um valor (serve para temp e corrente) */
float calcular_variacao(float atual, float anterior)
{
  return atual - anterior;
}

/* imprime a tendencia (subindo / descendo / estavel) */
void mostrar_tendencia(float variacao, String unidade)
{
  Serial.print("Variacao: ");
  Serial.print(variacao, 1);
  Serial.print(" ");
  Serial.println(unidade);

  if (variacao > 0)      Serial.println("  -> aumentando");
  else if (variacao < 0) Serial.println("  -> diminuindo");
  else                   Serial.println("  -> estavel");
}

void setup()
{
  Serial.begin(115200);
  sensor.begin();

  Serial.println();
  Serial.println("Monitor de superaquecimento eletrico");
  Serial.println("Temperatura (DS18B20) + Corrente (INA219 simulado)");
  Serial.println();

  Serial.print("Limite de temperatura: ");
  Serial.print(lim_temp);
  Serial.println(" C");
  Serial.print("Limite de corrente: ");
  Serial.print(lim_corrente);
  Serial.println(" mA");
  Serial.println("====================");
}

void loop()
{
  /* ---------- leitura da temperatura ---------- */
  sensor.requestTemperatures();
  float curr_temp = sensor.getTempCByIndex(0);

  /* ---------- leitura da corrente (potenciometro 0-4095 -> 0-2000 mA) ---------- */
  int leitura = analogRead(pino_pot);
  float curr_corrente = (leitura / 4095.0) * 2000.0;

  Serial.print("Temperatura: ");
  Serial.print(curr_temp, 1);
  Serial.println(" C");
  Serial.print("Corrente: ");
  Serial.print(curr_corrente, 1);
  Serial.println(" mA");

  /* ---------- define se cada um esta alto ---------- */
  bool temp_alta = (curr_temp >= lim_temp);
  bool corrente_alta = (curr_corrente >= lim_corrente);

  /* ---------- DIAGNOSTICO: cruza temperatura e corrente ---------- */
  Serial.print("Diagnostico: ");
  if (temp_alta && corrente_alta)
  {
    Serial.println("SOBRECARGA - o circuito puxa mais do que devia!");
  }
  else if (temp_alta && !corrente_alta)
  {
    Serial.println("MAU CONTATO - resistencia anormal, ponto gera calor sozinho!");
  }
  else if (!temp_alta && corrente_alta)
  {
    Serial.println("ALERTA PREVENTIVO - corrente alta, tendencia de risco.");
  }
  else
  {
    Serial.println("Normal - sistema operando dentro do esperado.");
  }

  /* ---------- tendencia (a partir da 2a leitura) ---------- */
  if (i_leitura > 0)
  {
    mostrar_tendencia(calcular_variacao(curr_temp, prev_temp), "C (temp)");
    mostrar_tendencia(calcular_variacao(curr_corrente, prev_corrente), "mA (corr)");
  }
  else
  {
    Serial.println("Primeira leitura.");
  }

  /* ---------- guarda para a proxima ---------- */
  prev_temp = curr_temp;
  prev_corrente = curr_corrente;
  i_leitura++;

  Serial.println("--------------------");
  delay(1500);
}
