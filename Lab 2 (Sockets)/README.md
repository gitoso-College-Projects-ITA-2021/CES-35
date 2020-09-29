# (CES-35) Lab 2: Sockets

**Regras:**
- Laboratório: Implementando Cliente e Servidor HTTP
- Em grupos de até 3 alunos
- Fazer uso do git (ou GitHub): Programas sem histórico de versão não serão considerados

**Instruções:**

Neste laboratório, o objetivo é ensinar programação de sockets através do design de um protocolo HTTP básico e por meio de desenvolvimento de um servidor e cliente Web simples. Todas as implementações devem ser escritas em C ou C++ usando a biblioteca padrão sockets BSD. Para o laboratório não será permitido bibliotecas de abstração de rede de mais alto nível como Boost.ASIO ou similares). O laboratório pode ser feito com uso de abstrações incluídas por padrão em C++11, como por exemplo, análise de string, multi-processos e multi-threading.

O laboratório possui 2 trechos de código a serem desenvolvidos: um arquivo contendo a implementação do servidor Web e um outro arquivo contendo o cliente Web. O servidor Web aceita uma solicitação HTTP, analisa a solicitação e procura o arquivo solicitado dentro do diretório de arquivos local. Se o arquivo solicitado existir, o servidor retornará o conteúdo do arquivo como parte de uma resposta HTTP, caso contrário, deverá retornar a resposta HTTP apropriada, com o código correspondente. O cliente ao recuperar a resposta Web deve salva-la em arquivo no diretório local. A ideia é implementar o equivalente ao HTTP 1.0, onde as conexões não são persistentes, é preciso um socket por objeto.

- Instruções completas disponíveis em: [sockets-laboratorio02-ces35-2020.pdf](docs/sockets-laboratorio02-ces35-2020.pdf)