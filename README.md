# DivingInventoryRFID
Code arduino et schema pour la version RFID de Diving Inventory

Materiel requis :
Trois led de couleurs différente      60 centimes
Trois résistance de 350 ohms          60 centimes
Un bouton poussoir                    3,64€ par 100
Un Module RFID RC522                  3€
Un  module bluetooth HC-06            3€
Un module Arduino UNO                 5€

Le module HC-06 doit être nommer diving
Pour cela, il faut taper dans le moniteur serie de l'IDE Arduino "AT+NAMEdiving"


Fonctionnement :
La READER_LED s'allume par default, il s'agit du mode lecture.
La WRITER-LED s'allume lors de l'appuis sur le bouton poussoir pour passer en mode création de badge.


Mode Lecture :
Le téléphone ou tablette doit étre coupler au lecteur, il suffit ensuite de vérifier que la READER_LED soit allumé et de passer le badge devant le lecteur, le numéro de matériel s'affichera alors sur le téléphone.

Mode Ecriture :
Il faut connecter le lecteur en USB au PC, lancer Diving Inventory PC et cliquer sur le bouton Badge NFC, choisir le type et le numéro du materiel, choisir le port de communication, passer le lecteur en mode ecriture (LED_WRITER allumé), poser le badge sur le module RFID, la STATUS_LED s'allume puis cliquez sur enregistrer. La STATUS_LED s'éteint.
