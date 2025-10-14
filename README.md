# CuraBot: Robot suiveur de ligne pour la livraison de médicaments

## Déscription du projet:
Ce projet consiste à concevoir et programmer un **robot suiveur de ligne autonome** capable d'accéder à différentes chambres pour déposer des
médicaments.
Ces chambres sont sélectionnées par l'utilisateur via un **écran tactile**, et le robot suit automatiquement un parcours déterminé par une ligne
noire au sol.
Ce projet a été réalisé lors de ma première année de formation en ingénierie robotique à Polytech Nice-Sophia (2024-2025).

---

## Fonctionnalités principales:
+ Suivi de ligne à l'aide de capteurs infrarouges
+ Livraison vers plusieurs destinations
+ Interface tactile pour la sélection des chambres
+ Retour visuel sur écran

---

## Technologies et composants utilisés:
| Type | Détails |
|------|---------|
| Microcontrôleur | Arduino Uno / Mega |
| Language | C / C++ (Arduino IDE) |
| Capteurs | Capteurs infrarouges et ultrasons |
| Actionneurs | Moteurs à courant continu + contrôleur Cytron |
| Interface utilisateur| Ecran tactile TFT |
| Autres | Batterie, 2 roues motrices, 1 roue folle |
