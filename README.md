__[FR] ●__ Ce dépôt contient trois implémentations (en C++20, en Python et en [Go][hp-golang])
d'un algorithme de recherche exhaustive (avec optimisations) permettant de déterminer
la solution minimale (c.-à-d. utilisant le minimum de déplacements) d'une grille
de [Klotski][wk-klotski].

Le résultat en sortie est un fichier SVG affichant la solution étape par étape,
similaire à ce qui suit :

![Solution étape par étape d'une grille de Klotski](c++/klotski_solution_expected.svg)

### Construire et exécuter l'implémentation en C++
Aucune bibliothèque ni dépendance externe n'est requise, un compilateur C++ prenant
en charge le standard C++20 suffira (notamment l'entête `<format>`).

Sous Linux, après avoir cloné le dépôt :
```
$ cd c++/
$ make
$ ./klotski_solver.prg
```
Cela génèrera un fichier `c++/klotski_solution.svg`, pouvant être visionné avec une
visionneuse d'images ou un navigateur web.

Sur d'autres plateformes, il suffira de compiler le fichier `c++/klotski_solver.cpp`
pour construire le binaire à exécuter.


### Exécuter l'implémentation en Python
N'importe quel interpréteur Python v3 récent suffira, aucune dépendance externe.
Testé avec Python 3.10.  Après avoir cloné le dépôt :
```
$ cd python/
$ python3 klotski_solver.py
```
Cela génèrera un fichier `python/klotski_solution.svg`, pouvant être visionné avec une
visionneuse d'images ou un navigateur web.


### Construire et exécuter l'implémentation en Go
Il suffira d'installer une version de Go >= 1.20, et de s'assurer que l'outil `go` est disponible
depuis la ligne de commande.  Après avoir cloné le dépôt :
```
$ cd go/
$ go build -o klotski_solver.prg klotski_solver.go    # ou simplement `make` si installé
$ ./klotski_solver.prg
```
Cela génèrera un fichier `go/klotski_solution.svg`, pouvant être visionné avec une
visionneuse d'images ou un navigateur web.


---

__[EN] ●__ This repository contains three implementations (in C++20, Python, and [Go][hp-golang])
of an exhaustive search algorithm (with many optimizations) that finds the shortest solution
(i.e. with the minimum amount of moves) of a [Klotski][wk-klotski] grid.


### Build and run the C++ implementation
No dependencies or external libraries are required.  A C++ compiler supporting C++20 is
all that is needed (in particular, the `<format>` header must be present).

On Linux, after cloning the repository:
```
$ cd c++/
$ make
$ ./klotski_solver.prg
```
This will generate an SVG image in `c++/klotski_solution.svg`, viewable using any
standard image viewer or web browser.

On other platforms, simply compile `c++/klotski_solver.cpp` to generate an executable
binary.


### Run the Python implementation
All that is needed is a modern Python v3 interpreter.  There are no external dependencies.
Tested with Python 3.10.  After cloning the repository:
```
$ cd python/
$ python3 klotski_solver.py
```
This will generate an SVG image in `python/klotski_solution.svg`, viewable using any
standard image viewer or web browser.


### Build and run the Go implementation
The Go toolchain >= 1.20 is needed.  Make sure the `go` binary is found in `PATH`.
After cloning the repository:
```
$ cd go/
$ go build -o klotski_solver.prg klotski_solver.go    # or simply `make`, if installed
$ ./klotski_solver.prg
```
This will generate an SVG image in `go/klotski_solution.svg`, viewable using any
standard image viewer or web browser.


[hp-golang]: https://golang.org/
[wk-klotski]: https://en.wikipedia.org/wiki/Klotski
