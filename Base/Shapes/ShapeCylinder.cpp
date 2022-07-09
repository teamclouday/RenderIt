#include "Shapes/MeshShapes.hpp"

namespace RenderIt
{

const std::string MeshShapeCylinderSource = R"(
# Blender v3.2.1 OBJ File: ''
# www.blender.org
o Cylinder
v 0.000000 -1.000000 -1.000000
v 0.000000 1.000000 -1.000000
v 0.195090 -1.000000 -0.980785
v 0.195090 1.000000 -0.980785
v 0.382683 -1.000000 -0.923880
v 0.382683 1.000000 -0.923880
v 0.555570 -1.000000 -0.831470
v 0.555570 1.000000 -0.831470
v 0.707107 -1.000000 -0.707107
v 0.707107 1.000000 -0.707107
v 0.831470 -1.000000 -0.555570
v 0.831470 1.000000 -0.555570
v 0.923880 -1.000000 -0.382683
v 0.923880 1.000000 -0.382683
v 0.980785 -1.000000 -0.195090
v 0.980785 1.000000 -0.195090
v 1.000000 -1.000000 0.000000
v 1.000000 1.000000 0.000000
v 0.980785 -1.000000 0.195090
v 0.980785 1.000000 0.195090
v 0.923880 -1.000000 0.382683
v 0.923880 1.000000 0.382683
v 0.831470 -1.000000 0.555570
v 0.831470 1.000000 0.555570
v 0.707107 -1.000000 0.707107
v 0.707107 1.000000 0.707107
v 0.555570 -1.000000 0.831470
v 0.555570 1.000000 0.831470
v 0.382683 -1.000000 0.923880
v 0.382683 1.000000 0.923880
v 0.195090 -1.000000 0.980785
v 0.195090 1.000000 0.980785
v -0.000000 -1.000000 1.000000
v -0.000000 1.000000 1.000000
v -0.195090 -1.000000 0.980785
v -0.195090 1.000000 0.980785
v -0.382683 -1.000000 0.923880
v -0.382683 1.000000 0.923880
v -0.555570 -1.000000 0.831470
v -0.555570 1.000000 0.831470
v -0.707107 -1.000000 0.707107
v -0.707107 1.000000 0.707107
v -0.831469 -1.000000 0.555570
v -0.831469 1.000000 0.555570
v -0.923880 -1.000000 0.382684
v -0.923880 1.000000 0.382684
v -0.980785 -1.000000 0.195090
v -0.980785 1.000000 0.195090
v -1.000000 -1.000000 -0.000000
v -1.000000 1.000000 -0.000000
v -0.980785 -1.000000 -0.195090
v -0.980785 1.000000 -0.195090
v -0.923879 -1.000000 -0.382684
v -0.923879 1.000000 -0.382684
v -0.831470 -1.000000 -0.555570
v -0.831470 1.000000 -0.555570
v -0.707107 -1.000000 -0.707107
v -0.707107 1.000000 -0.707107
v -0.555570 -1.000000 -0.831470
v -0.555570 1.000000 -0.831470
v -0.382683 -1.000000 -0.923880
v -0.382683 1.000000 -0.923880
v -0.195090 -1.000000 -0.980785
v -0.195090 1.000000 -0.980785
vt 1.000000 1.000000
vt 0.968750 0.500000
vt 1.000000 0.500000
vt 0.968750 1.000000
vt 0.937500 0.500000
vt 0.937500 1.000000
vt 0.906250 0.500000
vt 0.906250 1.000000
vt 0.875000 0.500000
vt 0.875000 1.000000
vt 0.843750 0.500000
vt 0.843750 1.000000
vt 0.812500 0.500000
vt 0.812500 1.000000
vt 0.781250 0.500000
vt 0.781250 1.000000
vt 0.750000 0.500000
vt 0.750000 1.000000
vt 0.718750 0.500000
vt 0.718750 1.000000
vt 0.687500 0.500000
vt 0.687500 1.000000
vt 0.656250 0.500000
vt 0.656250 1.000000
vt 0.625000 0.500000
vt 0.625000 1.000000
vt 0.593750 0.500000
vt 0.593750 1.000000
vt 0.562500 0.500000
vt 0.562500 1.000000
vt 0.531250 0.500000
vt 0.531250 1.000000
vt 0.500000 0.500000
vt 0.500000 1.000000
vt 0.468750 0.500000
vt 0.468750 1.000000
vt 0.437500 0.500000
vt 0.437500 1.000000
vt 0.406250 0.500000
vt 0.406250 1.000000
vt 0.375000 0.500000
vt 0.375000 1.000000
vt 0.343750 0.500000
vt 0.343750 1.000000
vt 0.312500 0.500000
vt 0.312500 1.000000
vt 0.281250 0.500000
vt 0.281250 1.000000
vt 0.250000 0.500000
vt 0.250000 1.000000
vt 0.218750 0.500000
vt 0.218750 1.000000
vt 0.187500 0.500000
vt 0.187500 1.000000
vt 0.156250 0.500000
vt 0.156250 1.000000
vt 0.125000 0.500000
vt 0.125000 1.000000
vt 0.093750 0.500000
vt 0.093750 1.000000
vt 0.062500 0.500000
vt 0.028269 0.341844
vt 0.158156 0.028269
vt 0.471731 0.158156
vt 0.062500 1.000000
vt 0.031250 0.500000
vt 0.031250 1.000000
vt 0.000000 0.500000
vt 0.985388 0.296822
vt 0.796822 0.014612
vt 0.514612 0.203178
vt 0.341844 0.471731
vt 0.296822 0.485388
vt 0.158156 0.471731
vt 0.250000 0.490000
vt 0.203178 0.485388
vt 0.116663 0.449553
vt 0.080294 0.419706
vt 0.050447 0.383337
vt 0.014612 0.296822
vt 0.010000 0.250000
vt 0.014612 0.203178
vt 0.028269 0.158156
vt 0.050447 0.116663
vt 0.080294 0.080294
vt 0.116663 0.050447
vt 0.203178 0.014612
vt 0.250000 0.010000
vt 0.296822 0.014612
vt 0.341844 0.028269
vt 0.383337 0.050447
vt 0.419706 0.080294
vt 0.449553 0.116663
vt 0.485388 0.203178
vt 0.490000 0.250000
vt 0.485388 0.296822
vt 0.471731 0.341844
vt 0.449553 0.383337
vt 0.419706 0.419706
vt 0.383337 0.449553
vt 0.000000 1.000000
vt 0.703178 0.485388
vt 0.750000 0.490000
vt 0.796822 0.485388
vt 0.841844 0.471731
vt 0.883337 0.449553
vt 0.919706 0.419706
vt 0.949553 0.383337
vt 0.971731 0.341844
vt 0.990000 0.250000
vt 0.985388 0.203178
vt 0.971731 0.158156
vt 0.949553 0.116663
vt 0.919706 0.080294
vt 0.883337 0.050447
vt 0.841844 0.028269
vt 0.750000 0.010000
vt 0.703178 0.014612
vt 0.658156 0.028269
vt 0.616663 0.050447
vt 0.580294 0.080294
vt 0.550447 0.116663
vt 0.528269 0.158156
vt 0.510000 0.250000
vt 0.514612 0.296822
vt 0.528269 0.341844
vt 0.550447 0.383337
vt 0.580294 0.419706
vt 0.616663 0.449553
vt 0.658156 0.471731
vn 0.0980 0.0000 -0.9952
vn 0.2903 0.0000 -0.9569
vn 0.4714 0.0000 -0.8819
vn 0.6344 0.0000 -0.7730
vn 0.7730 0.0000 -0.6344
vn 0.8819 0.0000 -0.4714
vn 0.9569 0.0000 -0.2903
vn 0.9952 0.0000 -0.0980
vn 0.9952 0.0000 0.0980
vn 0.9569 0.0000 0.2903
vn 0.8819 0.0000 0.4714
vn 0.7730 0.0000 0.6344
vn 0.6344 0.0000 0.7730
vn 0.4714 0.0000 0.8819
vn 0.2903 0.0000 0.9569
vn 0.0980 0.0000 0.9952
vn -0.0980 0.0000 0.9952
vn -0.2903 0.0000 0.9569
vn -0.4714 0.0000 0.8819
vn -0.6344 0.0000 0.7730
vn -0.7730 0.0000 0.6344
vn -0.8819 0.0000 0.4714
vn -0.9569 0.0000 0.2903
vn -0.9952 0.0000 0.0980
vn -0.9952 0.0000 -0.0980
vn -0.9569 0.0000 -0.2903
vn -0.8819 0.0000 -0.4714
vn -0.7730 0.0000 -0.6344
vn -0.6344 0.0000 -0.7730
vn -0.4714 0.0000 -0.8819
vn 0.0000 1.0000 0.0000
vn -0.2903 0.0000 -0.9569
vn -0.0980 0.0000 -0.9952
vn 0.0000 -1.0000 0.0000
s off
f 2/1/1 3/2/1 1/3/1
f 4/4/2 5/5/2 3/2/2
f 6/6/3 7/7/3 5/5/3
f 8/8/4 9/9/4 7/7/4
f 10/10/5 11/11/5 9/9/5
f 12/12/6 13/13/6 11/11/6
f 14/14/7 15/15/7 13/13/7
f 16/16/8 17/17/8 15/15/8
f 18/18/9 19/19/9 17/17/9
f 20/20/10 21/21/10 19/19/10
f 22/22/11 23/23/11 21/21/11
f 24/24/12 25/25/12 23/23/12
f 26/26/13 27/27/13 25/25/13
f 28/28/14 29/29/14 27/27/14
f 30/30/15 31/31/15 29/29/15
f 32/32/16 33/33/16 31/31/16
f 34/34/17 35/35/17 33/33/17
f 36/36/18 37/37/18 35/35/18
f 38/38/19 39/39/19 37/37/19
f 40/40/20 41/41/20 39/39/20
f 42/42/21 43/43/21 41/41/21
f 44/44/22 45/45/22 43/43/22
f 46/46/23 47/47/23 45/45/23
f 48/48/24 49/49/24 47/47/24
f 50/50/25 51/51/25 49/49/25
f 52/52/26 53/53/26 51/51/26
f 54/54/27 55/55/27 53/53/27
f 56/56/28 57/57/28 55/55/28
f 58/58/29 59/59/29 57/57/29
f 60/60/30 61/61/30 59/59/30
f 54/62/31 38/63/31 22/64/31
f 62/65/32 63/66/32 61/61/32
f 64/67/33 1/68/33 63/66/33
f 15/69/34 31/70/34 47/71/34
f 2/1/1 4/4/1 3/2/1
f 4/4/2 6/6/2 5/5/2
f 6/6/3 8/8/3 7/7/3
f 8/8/4 10/10/4 9/9/4
f 10/10/5 12/12/5 11/11/5
f 12/12/6 14/14/6 13/13/6
f 14/14/7 16/16/7 15/15/7
f 16/16/8 18/18/8 17/17/8
f 18/18/9 20/20/9 19/19/9
f 20/20/10 22/22/10 21/21/10
f 22/22/11 24/24/11 23/23/11
f 24/24/12 26/26/12 25/25/12
f 26/26/13 28/28/13 27/27/13
f 28/28/14 30/30/14 29/29/14
f 30/30/15 32/32/15 31/31/15
f 32/32/16 34/34/16 33/33/16
f 34/34/17 36/36/17 35/35/17
f 36/36/18 38/38/18 37/37/18
f 38/38/19 40/40/19 39/39/19
f 40/40/20 42/42/20 41/41/20
f 42/42/21 44/44/21 43/43/21
f 44/44/22 46/46/22 45/45/22
f 46/46/23 48/48/23 47/47/23
f 48/48/24 50/50/24 49/49/24
f 50/50/25 52/52/25 51/51/25
f 52/52/26 54/54/26 53/53/26
f 54/54/27 56/56/27 55/55/27
f 56/56/28 58/58/28 57/57/28
f 58/58/29 60/60/29 59/59/29
f 60/60/30 62/65/30 61/61/30
f 6/72/31 4/73/31 62/74/31
f 4/73/31 2/75/31 62/74/31
f 2/75/31 64/76/31 62/74/31
f 62/74/31 60/77/31 58/78/31
f 58/78/31 56/79/31 62/74/31
f 56/79/31 54/62/31 62/74/31
f 54/62/31 52/80/31 50/81/31
f 50/81/31 48/82/31 46/83/31
f 46/83/31 44/84/31 42/85/31
f 42/85/31 40/86/31 38/63/31
f 38/63/31 36/87/31 34/88/31
f 34/88/31 32/89/31 30/90/31
f 30/90/31 28/91/31 26/92/31
f 26/92/31 24/93/31 22/64/31
f 22/64/31 20/94/31 18/95/31
f 18/95/31 16/96/31 14/97/31
f 14/97/31 12/98/31 10/99/31
f 10/99/31 8/100/31 6/72/31
f 54/62/31 50/81/31 38/63/31
f 50/81/31 46/83/31 38/63/31
f 46/83/31 42/85/31 38/63/31
f 38/63/31 34/88/31 30/90/31
f 30/90/31 26/92/31 38/63/31
f 26/92/31 22/64/31 38/63/31
f 22/64/31 18/95/31 6/72/31
f 18/95/31 14/97/31 6/72/31
f 14/97/31 10/99/31 6/72/31
f 6/72/31 62/74/31 54/62/31
f 6/72/31 54/62/31 22/64/31
f 62/65/32 64/67/32 63/66/32
f 64/67/33 2/101/33 1/68/33
f 63/102/34 1/103/34 3/104/34
f 3/104/34 5/105/34 7/106/34
f 7/106/34 9/107/34 11/108/34
f 11/108/34 13/109/34 15/69/34
f 15/69/34 17/110/34 19/111/34
f 19/111/34 21/112/34 23/113/34
f 23/113/34 25/114/34 27/115/34
f 27/115/34 29/116/34 31/70/34
f 31/70/34 33/117/34 35/118/34
f 35/118/34 37/119/34 39/120/34
f 39/120/34 41/121/34 43/122/34
f 43/122/34 45/123/34 47/71/34
f 47/71/34 49/124/34 51/125/34
f 51/125/34 53/126/34 47/71/34
f 53/126/34 55/127/34 47/71/34
f 55/127/34 57/128/34 63/102/34
f 57/128/34 59/129/34 63/102/34
f 59/129/34 61/130/34 63/102/34
f 63/102/34 3/104/34 15/69/34
f 3/104/34 7/106/34 15/69/34
f 7/106/34 11/108/34 15/69/34
f 15/69/34 19/111/34 23/113/34
f 23/113/34 27/115/34 15/69/34
f 27/115/34 31/70/34 15/69/34
f 31/70/34 35/118/34 47/71/34
f 35/118/34 39/120/34 47/71/34
f 39/120/34 43/122/34 47/71/34
f 47/71/34 55/127/34 63/102/34
f 63/102/34 15/69/34 47/71/34
)";

} // namespace RenderIt
