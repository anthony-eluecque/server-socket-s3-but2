# Tic Tac Toe avec un serveur Linux en multijoueur


> **Auteurs :** ELUECQUE Anthony, CORION Gauthier


### **Présentation du projet** 
<br>
<p>
Projet réalisé lors du 3ème semestre de BUT Informatique, il s'agissait de réaliser un Tic-Tac-Toe en multijoueur utilisant un serveur linux. 
Il était demandé que celui-ci soit réalisé en C avec plusieurs étapes :

<table>
    <thead>
        <tr>
    </thead>
    <tbody>
        <tr>
            <td>N° version<br>
            <td align="center">Les objectifs à réaliser</td>
        </tr>
        <tr>
            <td>Version 0<br>
            <td>Le serveur et le client se communique mutellement les coups joués (sans respect des conditions de jeu (fin, grille pleine, ...)</td>
        </tr>
        <tr>
            <td>1ère version<br>
            <td>Le serveur et le client se communique mutuelle les coups joués + respect des règles du jeu avec ajouts d'une fin, du gagnant | perdant</td>
        </tr>
        <tr>
            <td>2ème version
            <td>Le serveur ne joue plus mais héberge une partie pour deux clients</td>
        </tr>
        <tr>
            <td>Bonus : 3ème version
            <td>Dès qu'une partie a commencé, tous les clients qui rejoignents sont des spectateurs</td>
        </tr>
        </tbody>
</table>

</p>
<br>

### **Exécuter le programme** ###


### `cd src`
### `gcc T3N_serveur.c -o serveur`
### `gcc T3N_client.c -o client`
<br>

# Sur un terminal, tapez : 
### ` ./serveur `
<br>
<p>Ouvrez deux autres terminaux et tapez dans chacun :
PORT = Valeur dans le fichier serveur (peut être modifier manuellement si bug</p>

### `./client 127.0.0.1 PORT `


