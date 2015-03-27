# Prérequis #
MinGW 4.4 ou supérieur, Visual Studio 2008 ou supérieur

# Ajouter ming dans le PATH #

  * Aller dans Panneau de configuration -> Système -> Avancé -> Variables d'environnement
  * Dans "variables système", sélectionner Path et cliquer sur Modifier
  * A la fin de Valeur variable, ajouter :
;C:\Qt\Qt5.3.1\Tools\mingw482\_32\bin

# Compilation de QSSH #
  * Télécharger et décompresser le fichier à la racine : https://github.com/lvklabs/QSsh/archive/master.zip
  * Créer un dossier et y aller : mkdir c:\build && cd c:\build
  * Exécuter : C:\Qt\Qt5.3.1\5.3\mingw482\_32\bin\qmake C:\QSsh-master\qssh.pro
  * Exécuter : mingw32-make.exe

Source : https://github.com/lvklabs/QSsh

# Installation de QSSH #
## A partir des fichiers venant d'être compilés ##
  * Aller dans c:\build\bin et copier les bibliothèques Botan.dll et QSsh.dll en Botand.dll et QSshd.dll.
  * Copier ces 4 fichiers dans le dossier de compilation de Ludopen (build)

## A partir des fichiers compilés à l'avance ##
  * télécharger le fichier ci-dessous et le décompresser dans le dossier de compilation de Ludopen (build)
http://ludotheque-savenay.hd.free.fr/ludopen/QSSH.zip