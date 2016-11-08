# find_the_number
Examen1 de Sistemas Operativos 1.

## server
va a estar esperando players se conecten, cuando se conecten al menos 2 players, crea una partida para ellos dos
escoje un numero random para que un player lo adivine.
y sigue esperando mas player para unirlos de dos en dos.

## player
se conecta al server y espera a su contrincante en caso de ser el primero.
cuando ambos estan conectados y el juego comienza, cada uno le manda al server su numero y el server les contesta:
`down` o `up` para que ellos depan si el numero esta mas abajo o mas arriba.
entonces el player hace una busqueda binaria para ello y le retorna el nuevo numero al player.

y asi hasta que alguien gane y mueren los dos procesos players.
