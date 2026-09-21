# Monitor de superaquecimento elétrico

Projeto de extensão / feira de tecnologia — solução contra superaquecimento em instalações elétricas (residências, condomínios e concessionárias), baseada em ESP32.

O sistema monitora **temperatura** e **corrente** ao mesmo tempo e **cruza as duas leituras** para dizer não apenas que há um problema, mas *qual* é o problema.

## Como funciona

- **Temperatura** — sensor DS18B20 (GPIO 2). Detecta o *efeito* da falha (o calor).
- **Corrente** — no protótipo real, sensor de corrente (INA219 para DC ou SCT-013 para AC). Detecta a *causa* (sobrecarga).
- **Diagnóstico** — o código combina as duas leituras e aplica a tabela abaixo.

## Tabela de diagnóstico

| Temperatura | Corrente | Diagnóstico |
|---|---|---|
| Alta | Alta | **SOBRECARGA** — o circuito puxa mais do que devia |
| Alta | Normal | **MAU CONTATO** — resistência anormal, o ponto gera calor sozinho |
| Normal | Alta | **ALERTA PREVENTIVO** — corrente alta, tendência de risco |
| Normal | Normal | Normal — sistema operando dentro do esperado |

Além do limite fixo, o sistema acompanha a **taxa de elevação (rate-of-rise)** — se o valor está subindo, descendo ou estável — para detectar a tendência de aquecimento antes de o valor absoluto chegar ao perigo.

## Simulação (Wokwi)

O Wokwi não possui o sensor de corrente (INA219/SCT-013), então na simulação a corrente é representada por um **potenciômetro** ligado ao pino analógico (GPIO 34). Isso permite testar toda a lógica — leitura, variação, limite e diagnóstico cruzado — girando o botão. No protótipo físico, o potenciômetro é substituído pelo sensor real; a lógica de decisão não muda.

Para rodar:
1. Cole o conteúdo de `sketch.ino`, `diagram.json` e `libraries.txt` num projeto ESP32 no Wokwi.
2. Rode a simulação.
3. Ajuste a temperatura clicando no DS18B20 e a "corrente" girando o potenciômetro para reproduzir cada cenário da tabela.

## Arquivos

- `sketch.ino` — código integrado (temperatura + corrente + diagnóstico).
- `diagram.json` — montagem no Wokwi (ESP32 + DS18B20 + potenciômetro).
- `libraries.txt` — bibliotecas usadas (OneWire, DallasTemperature).

## Observações para o protótipo real

- No hardware, a leitura da corrente troca `analogRead(...)` por um sensor real. Para corrente **contínua (DC)**: INA219, via I2C (SDA=21, SCL=22). Para corrente **alternada (AC)** da rede elétrica: SCT-013, sensor não invasivo (exige circuito de condicionamento e biblioteca de RMS).
- O DS18B20 no hardware precisa de resistor de pull-up de 4,7 kΩ entre DQ e VCC (na simulação do Wokwi não é necessário).
