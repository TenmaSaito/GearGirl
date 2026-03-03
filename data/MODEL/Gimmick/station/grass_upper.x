xof 0302txt 0064
template Header {
 <3D82AB43-62DA-11cf-AB39-0020AF71E433>
 WORD major;
 WORD minor;
 DWORD flags;
}

template Vector {
 <3D82AB5E-62DA-11cf-AB39-0020AF71E433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template Coords2d {
 <F6F23F44-7686-11cf-8F52-0040333594A3>
 FLOAT u;
 FLOAT v;
}

template Matrix4x4 {
 <F6F23F45-7686-11cf-8F52-0040333594A3>
 array FLOAT matrix[16];
}

template ColorRGBA {
 <35FF44E0-6C7C-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <D3E16E81-7835-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template IndexedColor {
 <1630B820-7842-11cf-8F52-0040333594A3>
 DWORD index;
 ColorRGBA indexColor;
}

template Boolean {
 <4885AE61-78E8-11cf-8F52-0040333594A3>
 WORD truefalse;
}

template Boolean2d {
 <4885AE63-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template MaterialWrap {
 <4885AE60-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template TextureFilename {
 <A42790E1-7810-11cf-8F52-0040333594A3>
 STRING filename;
}

template Material {
 <3D82AB4D-62DA-11cf-AB39-0020AF71E433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template MeshFace {
 <3D82AB5F-62DA-11cf-AB39-0020AF71E433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template MeshFaceWraps {
 <4885AE62-78E8-11cf-8F52-0040333594A3>
 DWORD nFaceWrapValues;
 Boolean2d faceWrapValues;
}

template MeshTextureCoords {
 <F6F23F40-7686-11cf-8F52-0040333594A3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}

template MeshMaterialList {
 <F6F23F42-7686-11cf-8F52-0040333594A3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material]
}

template MeshNormals {
 <F6F23F43-7686-11cf-8F52-0040333594A3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template MeshVertexColors {
 <1630B821-7842-11cf-8F52-0040333594A3>
 DWORD nVertexColors;
 array IndexedColor vertexColors[nVertexColors];
}

template Mesh {
 <3D82AB44-62DA-11cf-AB39-0020AF71E433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

Header{
1;
0;
1;
}

Mesh {
 24;
 -0.09613;62.61760;-48.89197;,
 -0.09613;62.61760;-47.66332;,
 -0.09613;57.26748;-47.66332;,
 -0.09620;62.61760;48.81870;,
 -0.09622;62.61760;49.97289;,
 -0.09622;54.22507;49.97289;,
 -0.09622;0.30088;49.97289;,
 -0.09613;0.30088;-47.66332;,
 0.88247;63.50468;48.59800;,
 0.88247;61.54448;48.86754;,
 0.88247;61.54448;-48.65616;,
 0.88247;63.50468;-49.10624;,
 -1.07773;61.54448;49.06522;,
 -1.07773;63.50468;48.79569;,
 -1.07773;63.50468;-49.08512;,
 -1.07773;61.54448;-48.63508;,
 -1.07773;63.50468;48.79569;,
 0.88247;63.50468;48.59800;,
 0.88247;63.50468;-49.10624;,
 -1.07773;63.50468;-49.08512;,
 0.88247;61.54448;48.86754;,
 -1.07773;61.54448;49.06522;,
 -1.07773;61.54448;-48.63508;,
 0.88247;61.54448;-48.65616;;
 
 12;
 3;0,1,2;,
 3;2,1,0;,
 3;3,4,5;,
 4;3,5,6,7;,
 3;3,7,1;,
 3;5,4,3;,
 4;5,3,1,7;,
 3;5,7,6;,
 4;8,9,10,11;,
 4;12,13,14,15;,
 4;16,17,18,19;,
 4;20,21,22,23;;
 
 MeshMaterialList {
  5;
  12;
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  4,
  0,
  0,
  0,
  0;;
  Material {
   0.000000;0.000000;0.000000;1.000000;;
   0.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
  }
  Material {
   0.020680;0.020680;0.020680;1.000000;;
   41.000000;
   0.590000;0.590000;0.590000;;
   0.000000;0.000000;0.000000;;
  }
  Material {
   0.696800;0.571200;0.320000;1.000000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
  }
  Material {
   0.615200;0.615200;0.615200;1.000000;;
   5.000000;
   0.600000;0.600000;0.600000;;
   0.000000;0.000000;0.000000;;
  }
  Material {
   0.800000;0.800000;0.800000;0.600000;;
   5.000000;
   0.300000;0.300000;0.300000;;
   0.000000;0.000000;0.000000;;
   TextureFilename {
    "C:\\Users\\student\\Downloads\\glass.jpg";
   }
  }
 }
 MeshNormals {
  16;
  1.000000;0.000000;0.000011;,
  1.000000;0.000000;0.000000;,
  1.000000;-0.000000;0.000003;,
  1.000000;-0.000000;0.000006;,
  -1.000000;0.000000;0.000000;,
  1.000000;0.000000;0.000000;,
  1.000000;-0.000000;0.000008;,
  1.000000;-0.000000;0.000006;,
  -1.000000;0.000000;-0.000001;,
  -1.000000;0.000000;-0.000004;,
  -1.000000;0.000000;-0.000011;,
  -1.000000;0.000000;-0.000006;,
  -1.000000;0.000000;-0.000001;,
  -1.000000;0.000000;-0.000001;,
  0.000000;1.000000;0.000000;,
  0.000000;-1.000000;-0.000000;;
  12;
  3;5,1,5;,
  3;4,8,4;,
  3;7,0,6;,
  4;7,6,3,2;,
  3;7,2,1;,
  3;9,10,11;,
  4;9,11,8,12;,
  3;9,12,13;,
  4;5,5,5,5;,
  4;4,4,4,4;,
  4;14,14,14,14;,
  4;15,15,15,15;;
 }
 MeshTextureCoords {
  24;
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;0.000000;,
  0.918396;0.000000;,
  0.918543;1.000000;,
  0.865397;1.000000;,
  0.865152;0.000000;,
  0.081350;1.000000;,
  0.081496;0.000000;,
  0.134836;0.000000;,
  0.134591;1.000000;,
  0.000000;0.081496;,
  1.000000;0.081604;,
  1.000000;0.134848;,
  0.000000;0.134836;,
  1.000000;0.918543;,
  0.000000;0.918651;,
  0.000000;0.865409;,
  1.000000;0.865397;;
 }
}
