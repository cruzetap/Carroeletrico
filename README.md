# Carroeletrico
controle virtual para um veiculo elétrico- LASD




Na primeira funcionalidade em questão, foi disposto da seguinte maneira, primeiramente foi acrescentado um controle (“joy stick”) para direcionar a simulação da seta do automóvel elétrico. De tal forma que, se a “lavanca” estiver mais posicionada para o lado direito, consequentemente o led associado a ele irá acender. Da mesma forma, se a o controle estiver direcionado mais para o outro lado, irá acender a seta/sinalização indicativa do lado esquerdo.
E como esperado, caso o controle se apresente centralizado ou em um posicionamento neutro, não haverá nenhuma sinalização.
Para obter essa implementação, no código foi feito as seguintes considerações.
Foi feita a conversão ADC.Onde o componente, tem a tensão no meio (centralizada ou posição neutra) de 2.5 V, significa que o controle/Joy stick está centralizado. Portanto, caso aplique algum comando que direcione ele para um lado ou para o outro, acontecerá uma variação de tensão, podendo oscilar de 0V a 5V.
Quando foi feita a conversão ADC, se obtive que caso fosse maior que 127V, estaria associado a “seta direita” que está interligada a porta C6. Assim como, caso essa variação fosse menor que 127, estaria associada a “seta esquerda” que está interligada a porta D7.


A segunda funcionalidade, seria um alarme sonoro, esse em questão não obtive sucesso na sua implementação.
Mas consiste, basicamente em emitir essa sonorização quando o sonar tivesse um valor menor que 350. A implementação foi feita no código , porém surgiu algumas interferências.


