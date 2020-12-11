# Lab 6: Mininet WiFi

- **Disciplina:** CES-35: Redes de Computadores e Internet
- **Aluno:** Gianluigi Dal Toso
- **Turma:** 2021
- **Data:** 11/12/2020

---

Neste relatório serão descritos os passos realizados ao seguir passo-a-passo dos procedimentos descritos na parte introdutória do livro "_Emulando Redes sem Fio com Mininet-WiFi_".

<!-- A execução do laboratório foi gravada no formato _asciinema_ (gravação do terminal, sem intervalo entre comandos). Para cada subseção à seguir, serão disponibilizados os links referentes as gravações. -->

O Laboratório foi executado em uma máquina virtual e portato executado no seguinte ambiente:

* Host:
    - **Sistema Operacional**: Arch Linux x64 (5.9.11-arch2-1)
    - **CPU**: Intel i7-6500U
    - **RAM**: 8 GB
    - **GPU**: Intel Skylake GT2 [HD Graphics 520]
* Guest (VM):
    - **Sistema Operacional**: Debian 10 (Buster) x64 (4.19.0-13-amd64)
    - **CPU**: Intel i7-6500U (1) @ 2.591GHz
    - **RAM**: 2 GB
    - **GPU**: VMware SVGA II Adapter

(Uma VM foi utilizada pois o Mininet-Wifi requer Debian, Ubuntu ou Fedora para a sua instalação)

Para cada sessão do relatórios, será inserida uma captura de tela referente indicando a execução do laboratório.

## <a name='TabeladeContedos'></a>Tabela de Conteúdos


<!-- vscode-markdown-toc -->

* [Tabela de Conteúdos](#TabeladeContedos)
* [2.1 - Primeiros passos com Mininet-WiFi](#PrimeirospassoscomMininet-WiFi)
* [2.2 - Customizando topologias](#Customizandotopologias)
* [2.3 - Acessando informações dos nós](#Acessandoinformaesdosns)
* [2.4 - OVSAP versus UserAP](#OVSAPversusUserAP)
* [2.5 - Utilizando interfaces gráficas](#Utilizandointerfacesgrficas)
	* [2.5.1 - Visual Network Descriptor](#VisualNetworkDescriptor)
	* [2.5.2 - MiniEdit](#MiniEdit)
	* [2.5.3 - Visualizando gráficos 2D e 3D](#Visualizandogrficos2De3D)
* [2.6 - Emulação do meio sem fio](#Emulaodomeiosemfio)
	* [2.6.1 - TC (Traffic Control)](#TCTrafficControl)
	* [2.6.2 - Wmediumd](#Wmediumd)
	* [2.6.3 - TC versus Wmediumd na prática](#TCversusWmediumdnaprtica)
* [2.7 - Modelos de propagação](#Modelosdepropagao)
	* [2.7.1 - Provendo mais realismo](#Provendomaisrealismo)

<!-- vscode-markdown-toc-config
	numbering=false
	autoSave=true
	/vscode-markdown-toc-config -->
<!-- /vscode-markdown-toc -->

---

# Pré-Lab
Instalação do Mininet-WiFi:

Instruções do GitHub do projeto:
```
$ sudo apt-get install git
$ git clone https://github.com/intrig-unicamp/mininet-wifi
$ cd mininet-wifi
$ sudo util/install.sh -Wlnfv
```


---
**Screenshot:**
![pre](imagens/pre.png)

# 2 - Iniciante

## <a name='PrimeirospassoscomMininet-WiFi'></a>2.1 - Primeiros passos com Mininet-WiFi

---
**Screenshots:**
![](imagens/211.png)
![](imagens/212.png)
![](imagens/213.png)
![](imagens/214.png)
![](imagens/215.png)
![](imagens/216.png)
![](imagens/217.png)
![](imagens/218.png)
![](imagens/219.png)
![](imagens/2110.png)
![](imagens/2111.png)
![](imagens/2112.png)
![](imagens/2113.png)

## <a name='Customizandotopologias'></a>2.2 - Customizando topologias

---
**Screenshots:**
![](imagens/221.png)
![](imagens/222.png)
![](imagens/223.png)

## <a name='Acessandoinformaesdosns'></a>2.3 - Acessando informações dos nós

---
**Screenshots:**
![](imagens/231.png)
![](imagens/232.png)
![](imagens/233.png)
![](imagens/234.png)
![](imagens/235.png)
![](imagens/236.png)

## <a name='OVSAPversusUserAP'></a>2.4 - OVSAP versus UserAP

---
**Screenshots:**
![](imagens/241.png)
![](imagens/242.png)
![](imagens/243.png)
![](imagens/244.png)
![](imagens/245.png)
![](imagens/246.png)

## <a name='Utilizandointerfacesgrficas'></a>2.5 - Utilizando interfaces gráficas

### <a name='VisualNetworkDescriptor'></a>2.5.1 - Visual Network Descriptor

A instalação do VND envolveu os seguintes passos adicionais:

```
git clone https://github.com/ramonfontes/vnd.git
sudo apt install apache2
sudo systemctl start apache2
sudo mv /home/gitoso/mininet-wifi/vnd/* .
```

Adicionalmente foi necessário instalar o Adobe Flash Player

---
**Screenshots:**
![](imagens/2511.png)
![](imagens/2512.png)

### <a name='MiniEdit'></a>2.5.2 - MiniEdit

---
**Screenshots:**
![](imagens/2521.png)
![](imagens/2522.png)


### <a name='Visualizandogrficos2De3D'></a>2.5.3 - Visualizando gráficos 2D e 3D

---
**Screenshots:**
![](imagens/2532.png)
![](imagens/2531.png)

## <a name='Emulaodomeiosemfio'></a>2.6 - Emulação do meio sem fio

### <a name='TCTrafficControl'></a>2.6.1 - TC (Traffic Control)

---
**Screenshots:**
![](imagens/2611.png)

### <a name='Wmediumd'></a>2.6.2 - Wmediumd

(Só teoria)

### <a name='TCversusWmediumdnaprtica'></a>2.6.3 - TC versus Wmediumd na prática

---
**Screenshots:**
![](imagens/2631.png)
![](imagens/2632.png)
![](imagens/2633.png)
![](imagens/2634.png)
![](imagens/2635.png)
![](imagens/2636.png)

## <a name='Modelosdepropagao'></a>2.7 - Modelos de propagação

---
**Screenshots:**
![](imagens/271.png)
![](imagens/272.png)
![](imagens/273.png)
![](imagens/274.png)

### <a name='Provendomaisrealismo'></a>2.7.1 - Provendo mais realismo

Bem, aqui tive um problema e deixei o laboratório de lado. Depois a atividade acabou ficando opcional e por estar sobrecarregado com demandas optei por não continuar. De qualquer forma, para não desperdiçar o trabalho parcial que já havia realizado, optei por realizar uma entrega incompleta mesmo.