#include "Shapes/MeshShapes.hpp"

namespace RenderIt
{

const std::string MeshShapeCylinderSource = "# Blender v3.2.1 OBJ File: ''\n"
                                            "# www.blender.org\n"
                                            "o Cylinder_Cylinder.001\n"
                                            "v 0.000000 -1.000000 -1.000000\n"
                                            "v 0.000000 1.000000 -1.000000\n"
                                            "v 0.195090 -1.000000 -0.980785\n"
                                            "v 0.195090 1.000000 -0.980785\n"
                                            "v 0.382683 -1.000000 -0.923880\n"
                                            "v 0.382683 1.000000 -0.923880\n"
                                            "v 0.555570 -1.000000 -0.831470\n"
                                            "v 0.555570 1.000000 -0.831470\n"
                                            "v 0.707107 -1.000000 -0.707107\n"
                                            "v 0.707107 1.000000 -0.707107\n"
                                            "v 0.831470 -1.000000 -0.555570\n"
                                            "v 0.831470 1.000000 -0.555570\n"
                                            "v 0.923880 -1.000000 -0.382683\n"
                                            "v 0.923880 1.000000 -0.382683\n"
                                            "v 0.980785 -1.000000 -0.195090\n"
                                            "v 0.980785 1.000000 -0.195090\n"
                                            "v 1.000000 -1.000000 0.000000\n"
                                            "v 1.000000 1.000000 0.000000\n"
                                            "v 0.980785 -1.000000 0.195090\n"
                                            "v 0.980785 1.000000 0.195090\n"
                                            "v 0.923880 -1.000000 0.382683\n"
                                            "v 0.923880 1.000000 0.382683\n"
                                            "v 0.831470 -1.000000 0.555570\n"
                                            "v 0.831470 1.000000 0.555570\n"
                                            "v 0.707107 -1.000000 0.707107\n"
                                            "v 0.707107 1.000000 0.707107\n"
                                            "v 0.555570 -1.000000 0.831470\n"
                                            "v 0.555570 1.000000 0.831470\n"
                                            "v 0.382683 -1.000000 0.923880\n"
                                            "v 0.382683 1.000000 0.923880\n"
                                            "v 0.195090 -1.000000 0.980785\n"
                                            "v 0.195090 1.000000 0.980785\n"
                                            "v -0.000000 -1.000000 1.000000\n"
                                            "v -0.000000 1.000000 1.000000\n"
                                            "v -0.195090 -1.000000 0.980785\n"
                                            "v -0.195090 1.000000 0.980785\n"
                                            "v -0.382683 -1.000000 0.923880\n"
                                            "v -0.382683 1.000000 0.923880\n"
                                            "v -0.555570 -1.000000 0.831470\n"
                                            "v -0.555570 1.000000 0.831470\n"
                                            "v -0.707107 -1.000000 0.707107\n"
                                            "v -0.707107 1.000000 0.707107\n"
                                            "v -0.831469 -1.000000 0.555570\n"
                                            "v -0.831469 1.000000 0.555570\n"
                                            "v -0.923880 -1.000000 0.382684\n"
                                            "v -0.923880 1.000000 0.382684\n"
                                            "v -0.980785 -1.000000 0.195090\n"
                                            "v -0.980785 1.000000 0.195090\n"
                                            "v -1.000000 -1.000000 -0.000000\n"
                                            "v -1.000000 1.000000 -0.000000\n"
                                            "v -0.980785 -1.000000 -0.195090\n"
                                            "v -0.980785 1.000000 -0.195090\n"
                                            "v -0.923879 -1.000000 -0.382684\n"
                                            "v -0.923879 1.000000 -0.382684\n"
                                            "v -0.831470 -1.000000 -0.555570\n"
                                            "v -0.831470 1.000000 -0.555570\n"
                                            "v -0.707107 -1.000000 -0.707107\n"
                                            "v -0.707107 1.000000 -0.707107\n"
                                            "v -0.555570 -1.000000 -0.831470\n"
                                            "v -0.555570 1.000000 -0.831470\n"
                                            "v -0.382683 -1.000000 -0.923880\n"
                                            "v -0.382683 1.000000 -0.923880\n"
                                            "v -0.195090 -1.000000 -0.980785\n"
                                            "v -0.195090 1.000000 -0.980785\n"
                                            "v 0.000000 -1.000000 -1.000000\n"
                                            "v 0.195090 -1.000000 -0.980785\n"
                                            "v 0.195090 1.000000 -0.980785\n"
                                            "v 0.000000 1.000000 -1.000000\n"
                                            "v 0.382683 -1.000000 -0.923880\n"
                                            "v 0.382683 1.000000 -0.923880\n"
                                            "v 0.555570 -1.000000 -0.831470\n"
                                            "v 0.555570 1.000000 -0.831470\n"
                                            "v 0.707107 -1.000000 -0.707107\n"
                                            "v 0.707107 1.000000 -0.707107\n"
                                            "v 0.831470 -1.000000 -0.555570\n"
                                            "v 0.831470 1.000000 -0.555570\n"
                                            "v 0.923880 -1.000000 -0.382683\n"
                                            "v 0.923880 1.000000 -0.382683\n"
                                            "v 0.980785 -1.000000 -0.195090\n"
                                            "v 0.980785 1.000000 -0.195090\n"
                                            "v 1.000000 -1.000000 0.000000\n"
                                            "v 1.000000 1.000000 0.000000\n"
                                            "v 0.980785 -1.000000 0.195090\n"
                                            "v 0.980785 1.000000 0.195090\n"
                                            "v 0.923880 -1.000000 0.382683\n"
                                            "v 0.923880 1.000000 0.382683\n"
                                            "v 0.831470 -1.000000 0.555570\n"
                                            "v 0.831470 1.000000 0.555570\n"
                                            "v 0.707107 -1.000000 0.707107\n"
                                            "v 0.707107 1.000000 0.707107\n"
                                            "v 0.555570 -1.000000 0.831470\n"
                                            "v 0.555570 1.000000 0.831470\n"
                                            "v 0.382683 -1.000000 0.923880\n"
                                            "v 0.382683 1.000000 0.923880\n"
                                            "v 0.195090 -1.000000 0.980785\n"
                                            "v 0.195090 1.000000 0.980785\n"
                                            "v -0.000000 -1.000000 1.000000\n"
                                            "v -0.000000 1.000000 1.000000\n"
                                            "v -0.195090 -1.000000 0.980785\n"
                                            "v -0.195090 1.000000 0.980785\n"
                                            "v -0.382683 -1.000000 0.923880\n"
                                            "v -0.382683 1.000000 0.923880\n"
                                            "v -0.555570 -1.000000 0.831470\n"
                                            "v -0.555570 1.000000 0.831470\n"
                                            "v -0.707107 -1.000000 0.707107\n"
                                            "v -0.707107 1.000000 0.707107\n"
                                            "v -0.831469 -1.000000 0.555570\n"
                                            "v -0.831469 1.000000 0.555570\n"
                                            "v -0.923880 -1.000000 0.382684\n"
                                            "v -0.923880 1.000000 0.382684\n"
                                            "v -0.980785 -1.000000 0.195090\n"
                                            "v -0.980785 1.000000 0.195090\n"
                                            "v -1.000000 -1.000000 -0.000000\n"
                                            "v -1.000000 1.000000 -0.000000\n"
                                            "v -0.980785 -1.000000 -0.195090\n"
                                            "v -0.980785 1.000000 -0.195090\n"
                                            "v -0.923879 -1.000000 -0.382684\n"
                                            "v -0.923879 1.000000 -0.382684\n"
                                            "v -0.831470 -1.000000 -0.555570\n"
                                            "v -0.831470 1.000000 -0.555570\n"
                                            "v -0.707107 -1.000000 -0.707107\n"
                                            "v -0.707107 1.000000 -0.707107\n"
                                            "v -0.555570 -1.000000 -0.831470\n"
                                            "v -0.555570 1.000000 -0.831470\n"
                                            "v -0.382683 -1.000000 -0.923880\n"
                                            "v -0.382683 1.000000 -0.923880\n"
                                            "v -0.195090 -1.000000 -0.980785\n"
                                            "v -0.195090 1.000000 -0.980785\n"
                                            "vt 1.000000 1.000000\n"
                                            "vt 0.968750 0.500000\n"
                                            "vt 1.000000 0.500000\n"
                                            "vt 0.968750 1.000000\n"
                                            "vt 0.937500 0.500000\n"
                                            "vt 0.937500 1.000000\n"
                                            "vt 0.906250 0.500000\n"
                                            "vt 0.906250 1.000000\n"
                                            "vt 0.875000 0.500000\n"
                                            "vt 0.875000 1.000000\n"
                                            "vt 0.843750 0.500000\n"
                                            "vt 0.843750 1.000000\n"
                                            "vt 0.812500 0.500000\n"
                                            "vt 0.812500 1.000000\n"
                                            "vt 0.781250 0.500000\n"
                                            "vt 0.781250 1.000000\n"
                                            "vt 0.750000 0.500000\n"
                                            "vt 0.750000 1.000000\n"
                                            "vt 0.718750 0.500000\n"
                                            "vt 0.718750 1.000000\n"
                                            "vt 0.687500 0.500000\n"
                                            "vt 0.687500 1.000000\n"
                                            "vt 0.656250 0.500000\n"
                                            "vt 0.656250 1.000000\n"
                                            "vt 0.625000 0.500000\n"
                                            "vt 0.625000 1.000000\n"
                                            "vt 0.593750 0.500000\n"
                                            "vt 0.593750 1.000000\n"
                                            "vt 0.562500 0.500000\n"
                                            "vt 0.562500 1.000000\n"
                                            "vt 0.531250 0.500000\n"
                                            "vt 0.531250 1.000000\n"
                                            "vt 0.500000 0.500000\n"
                                            "vt 0.500000 1.000000\n"
                                            "vt 0.468750 0.500000\n"
                                            "vt 0.468750 1.000000\n"
                                            "vt 0.437500 0.500000\n"
                                            "vt 0.437500 1.000000\n"
                                            "vt 0.406250 0.500000\n"
                                            "vt 0.406250 1.000000\n"
                                            "vt 0.375000 0.500000\n"
                                            "vt 0.375000 1.000000\n"
                                            "vt 0.343750 0.500000\n"
                                            "vt 0.343750 1.000000\n"
                                            "vt 0.312500 0.500000\n"
                                            "vt 0.312500 1.000000\n"
                                            "vt 0.281250 0.500000\n"
                                            "vt 0.281250 1.000000\n"
                                            "vt 0.250000 0.500000\n"
                                            "vt 0.250000 1.000000\n"
                                            "vt 0.218750 0.500000\n"
                                            "vt 0.218750 1.000000\n"
                                            "vt 0.187500 0.500000\n"
                                            "vt 0.187500 1.000000\n"
                                            "vt 0.156250 0.500000\n"
                                            "vt 0.156250 1.000000\n"
                                            "vt 0.125000 0.500000\n"
                                            "vt 0.125000 1.000000\n"
                                            "vt 0.093750 0.500000\n"
                                            "vt 0.093750 1.000000\n"
                                            "vt 0.062500 0.500000\n"
                                            "vt 0.028269 0.341844\n"
                                            "vt 0.158156 0.028269\n"
                                            "vt 0.471731 0.158156\n"
                                            "vt 0.062500 1.000000\n"
                                            "vt 0.031250 0.500000\n"
                                            "vt 0.031250 1.000000\n"
                                            "vt 0.000000 0.500000\n"
                                            "vt 0.985388 0.296822\n"
                                            "vt 0.796822 0.014612\n"
                                            "vt 0.514612 0.203178\n"
                                            "vt 0.341844 0.471731\n"
                                            "vt 0.296822 0.485388\n"
                                            "vt 0.158156 0.471731\n"
                                            "vt 0.250000 0.490000\n"
                                            "vt 0.203178 0.485388\n"
                                            "vt 0.116663 0.449553\n"
                                            "vt 0.080294 0.419706\n"
                                            "vt 0.050447 0.383337\n"
                                            "vt 0.014612 0.296822\n"
                                            "vt 0.010000 0.250000\n"
                                            "vt 0.014612 0.203178\n"
                                            "vt 0.028269 0.158156\n"
                                            "vt 0.050447 0.116663\n"
                                            "vt 0.080294 0.080294\n"
                                            "vt 0.116663 0.050447\n"
                                            "vt 0.203178 0.014612\n"
                                            "vt 0.250000 0.010000\n"
                                            "vt 0.296822 0.014612\n"
                                            "vt 0.341844 0.028269\n"
                                            "vt 0.383337 0.050447\n"
                                            "vt 0.419706 0.080294\n"
                                            "vt 0.449553 0.116663\n"
                                            "vt 0.485388 0.203178\n"
                                            "vt 0.490000 0.250000\n"
                                            "vt 0.485388 0.296822\n"
                                            "vt 0.471731 0.341844\n"
                                            "vt 0.449553 0.383337\n"
                                            "vt 0.419706 0.419706\n"
                                            "vt 0.383337 0.449553\n"
                                            "vt 0.000000 1.000000\n"
                                            "vt 0.703178 0.485388\n"
                                            "vt 0.750000 0.490000\n"
                                            "vt 0.796822 0.485388\n"
                                            "vt 0.841844 0.471731\n"
                                            "vt 0.883337 0.449553\n"
                                            "vt 0.919706 0.419706\n"
                                            "vt 0.949553 0.383337\n"
                                            "vt 0.971731 0.341844\n"
                                            "vt 0.990000 0.250000\n"
                                            "vt 0.985388 0.203178\n"
                                            "vt 0.971731 0.158156\n"
                                            "vt 0.949553 0.116663\n"
                                            "vt 0.919706 0.080294\n"
                                            "vt 0.883337 0.050447\n"
                                            "vt 0.841844 0.028269\n"
                                            "vt 0.750000 0.010000\n"
                                            "vt 0.703178 0.014612\n"
                                            "vt 0.658156 0.028269\n"
                                            "vt 0.616663 0.050447\n"
                                            "vt 0.580294 0.080294\n"
                                            "vt 0.550447 0.116663\n"
                                            "vt 0.528269 0.158156\n"
                                            "vt 0.510000 0.250000\n"
                                            "vt 0.514612 0.296822\n"
                                            "vt 0.528269 0.341844\n"
                                            "vt 0.550447 0.383337\n"
                                            "vt 0.580294 0.419706\n"
                                            "vt 0.616663 0.449553\n"
                                            "vt 0.658156 0.471731\n"
                                            "vn 0.0000 0.0000 -1.0000\n"
                                            "vn 0.1951 0.0000 -0.9808\n"
                                            "vn 0.3827 0.0000 -0.9239\n"
                                            "vn 0.5556 0.0000 -0.8315\n"
                                            "vn 0.7071 0.0000 -0.7071\n"
                                            "vn 0.8315 0.0000 -0.5556\n"
                                            "vn 0.9239 0.0000 -0.3827\n"
                                            "vn 0.9808 0.0000 -0.1951\n"
                                            "vn 1.0000 0.0000 0.0000\n"
                                            "vn 0.9808 0.0000 0.1951\n"
                                            "vn 0.9239 0.0000 0.3827\n"
                                            "vn 0.8315 0.0000 0.5556\n"
                                            "vn 0.7071 0.0000 0.7071\n"
                                            "vn 0.5556 0.0000 0.8315\n"
                                            "vn 0.3827 0.0000 0.9239\n"
                                            "vn 0.1951 0.0000 0.9808\n"
                                            "vn 0.0000 0.0000 1.0000\n"
                                            "vn -0.1951 0.0000 0.9808\n"
                                            "vn -0.3827 0.0000 0.9239\n"
                                            "vn -0.5556 0.0000 0.8315\n"
                                            "vn -0.7071 0.0000 0.7071\n"
                                            "vn -0.8315 0.0000 0.5556\n"
                                            "vn -0.9239 0.0000 0.3827\n"
                                            "vn -0.9808 0.0000 0.1951\n"
                                            "vn -1.0000 0.0000 0.0000\n"
                                            "vn -0.9808 0.0000 -0.1951\n"
                                            "vn -0.9239 0.0000 -0.3827\n"
                                            "vn -0.8315 0.0000 -0.5556\n"
                                            "vn -0.7071 0.0000 -0.7071\n"
                                            "vn -0.5556 0.0000 -0.8315\n"
                                            "vn -0.3827 0.0000 -0.9239\n"
                                            "vn 0.0000 1.0000 -0.0000\n"
                                            "vn -0.1951 0.0000 -0.9808\n"
                                            "vn 0.0000 -1.0000 0.0000\n"
                                            "s 1\n"
                                            "f 68/1/1 66/2/2 65/3/1\n"
                                            "f 67/4/2 69/5/3 66/2/2\n"
                                            "f 70/6/3 71/7/4 69/5/3\n"
                                            "f 72/8/4 73/9/5 71/7/4\n"
                                            "f 74/10/5 75/11/6 73/9/5\n"
                                            "f 76/12/6 77/13/7 75/11/6\n"
                                            "f 78/14/7 79/15/8 77/13/7\n"
                                            "f 80/16/8 81/17/9 79/15/8\n"
                                            "f 82/18/9 83/19/10 81/17/9\n"
                                            "f 84/20/10 85/21/11 83/19/10\n"
                                            "f 86/22/11 87/23/12 85/21/11\n"
                                            "f 88/24/12 89/25/13 87/23/12\n"
                                            "f 90/26/13 91/27/14 89/25/13\n"
                                            "f 92/28/14 93/29/15 91/27/14\n"
                                            "f 94/30/15 95/31/16 93/29/15\n"
                                            "f 96/32/16 97/33/17 95/31/16\n"
                                            "f 98/34/17 99/35/18 97/33/17\n"
                                            "f 100/36/18 101/37/19 99/35/18\n"
                                            "f 102/38/19 103/39/20 101/37/19\n"
                                            "f 104/40/20 105/41/21 103/39/20\n"
                                            "f 106/42/21 107/43/22 105/41/21\n"
                                            "f 108/44/22 109/45/23 107/43/22\n"
                                            "f 110/46/23 111/47/24 109/45/23\n"
                                            "f 112/48/24 113/49/25 111/47/24\n"
                                            "f 114/50/25 115/51/26 113/49/25\n"
                                            "f 116/52/26 117/53/27 115/51/26\n"
                                            "f 118/54/27 119/55/28 117/53/27\n"
                                            "f 120/56/28 121/57/29 119/55/28\n"
                                            "f 122/58/29 123/59/30 121/57/29\n"
                                            "f 124/60/30 125/61/31 123/59/30\n"
                                            "f 54/62/32 38/63/32 22/64/32\n"
                                            "f 126/65/31 127/66/33 125/61/31\n"
                                            "f 64/67/33 65/68/1 127/66/33\n"
                                            "f 15/69/34 31/70/34 47/71/34\n"
                                            "f 68/1/1 67/4/2 66/2/2\n"
                                            "f 67/4/2 70/6/3 69/5/3\n"
                                            "f 70/6/3 72/8/4 71/7/4\n"
                                            "f 72/8/4 74/10/5 73/9/5\n"
                                            "f 74/10/5 76/12/6 75/11/6\n"
                                            "f 76/12/6 78/14/7 77/13/7\n"
                                            "f 78/14/7 80/16/8 79/15/8\n"
                                            "f 80/16/8 82/18/9 81/17/9\n"
                                            "f 82/18/9 84/20/10 83/19/10\n"
                                            "f 84/20/10 86/22/11 85/21/11\n"
                                            "f 86/22/11 88/24/12 87/23/12\n"
                                            "f 88/24/12 90/26/13 89/25/13\n"
                                            "f 90/26/13 92/28/14 91/27/14\n"
                                            "f 92/28/14 94/30/15 93/29/15\n"
                                            "f 94/30/15 96/32/16 95/31/16\n"
                                            "f 96/32/16 98/34/17 97/33/17\n"
                                            "f 98/34/17 100/36/18 99/35/18\n"
                                            "f 100/36/18 102/38/19 101/37/19\n"
                                            "f 102/38/19 104/40/20 103/39/20\n"
                                            "f 104/40/20 106/42/21 105/41/21\n"
                                            "f 106/42/21 108/44/22 107/43/22\n"
                                            "f 108/44/22 110/46/23 109/45/23\n"
                                            "f 110/46/23 112/48/24 111/47/24\n"
                                            "f 112/48/24 114/50/25 113/49/25\n"
                                            "f 114/50/25 116/52/26 115/51/26\n"
                                            "f 116/52/26 118/54/27 117/53/27\n"
                                            "f 118/54/27 120/56/28 119/55/28\n"
                                            "f 120/56/28 122/58/29 121/57/29\n"
                                            "f 122/58/29 124/60/30 123/59/30\n"
                                            "f 124/60/30 126/65/31 125/61/31\n"
                                            "f 6/72/32 4/73/32 62/74/32\n"
                                            "f 4/73/32 2/75/32 62/74/32\n"
                                            "f 2/75/32 128/76/32 62/74/32\n"
                                            "f 62/74/32 60/77/32 58/78/32\n"
                                            "f 58/78/32 56/79/32 62/74/32\n"
                                            "f 56/79/32 54/62/32 62/74/32\n"
                                            "f 54/62/32 52/80/32 50/81/32\n"
                                            "f 50/81/32 48/82/32 46/83/32\n"
                                            "f 46/83/32 44/84/32 42/85/32\n"
                                            "f 42/85/32 40/86/32 38/63/32\n"
                                            "f 38/63/32 36/87/32 34/88/32\n"
                                            "f 34/88/32 32/89/32 30/90/32\n"
                                            "f 30/90/32 28/91/32 26/92/32\n"
                                            "f 26/92/32 24/93/32 22/64/32\n"
                                            "f 22/64/32 20/94/32 18/95/32\n"
                                            "f 18/95/32 16/96/32 14/97/32\n"
                                            "f 14/97/32 12/98/32 10/99/32\n"
                                            "f 10/99/32 8/100/32 6/72/32\n"
                                            "f 54/62/32 50/81/32 38/63/32\n"
                                            "f 50/81/32 46/83/32 38/63/32\n"
                                            "f 46/83/32 42/85/32 38/63/32\n"
                                            "f 38/63/32 34/88/32 30/90/32\n"
                                            "f 30/90/32 26/92/32 38/63/32\n"
                                            "f 26/92/32 22/64/32 38/63/32\n"
                                            "f 22/64/32 18/95/32 6/72/32\n"
                                            "f 18/95/32 14/97/32 6/72/32\n"
                                            "f 14/97/32 10/99/32 6/72/32\n"
                                            "f 6/72/32 62/74/32 54/62/32\n"
                                            "f 6/72/32 54/62/32 22/64/32\n"
                                            "f 126/65/31 64/67/33 127/66/33\n"
                                            "f 64/67/33 68/101/1 65/68/1\n"
                                            "f 63/102/34 1/103/34 3/104/34\n"
                                            "f 3/104/34 5/105/34 7/106/34\n"
                                            "f 7/106/34 9/107/34 11/108/34\n"
                                            "f 11/108/34 13/109/34 15/69/34\n"
                                            "f 15/69/34 17/110/34 19/111/34\n"
                                            "f 19/111/34 21/112/34 23/113/34\n"
                                            "f 23/113/34 25/114/34 27/115/34\n"
                                            "f 27/115/34 29/116/34 31/70/34\n"
                                            "f 31/70/34 33/117/34 35/118/34\n"
                                            "f 35/118/34 37/119/34 39/120/34\n"
                                            "f 39/120/34 41/121/34 43/122/34\n"
                                            "f 43/122/34 45/123/34 47/71/34\n"
                                            "f 47/71/34 49/124/34 51/125/34\n"
                                            "f 51/125/34 53/126/34 47/71/34\n"
                                            "f 53/126/34 55/127/34 47/71/34\n"
                                            "f 55/127/34 57/128/34 63/102/34\n"
                                            "f 57/128/34 59/129/34 63/102/34\n"
                                            "f 59/129/34 61/130/34 63/102/34\n"
                                            "f 63/102/34 3/104/34 15/69/34\n"
                                            "f 3/104/34 7/106/34 15/69/34\n"
                                            "f 7/106/34 11/108/34 15/69/34\n"
                                            "f 15/69/34 19/111/34 23/113/34\n"
                                            "f 23/113/34 27/115/34 15/69/34\n"
                                            "f 27/115/34 31/70/34 15/69/34\n"
                                            "f 31/70/34 35/118/34 47/71/34\n"
                                            "f 35/118/34 39/120/34 47/71/34\n"
                                            "f 39/120/34 43/122/34 47/71/34\n"
                                            "f 47/71/34 55/127/34 63/102/34\n"
                                            "f 63/102/34 15/69/34 47/71/34\n";

} // namespace RenderIt
