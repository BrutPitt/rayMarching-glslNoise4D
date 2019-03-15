////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2018 Michele Morrone
//  All rights reserved.
//
//  mailto:me@michelemorrone.eu
//  mailto:brutpitt@gmail.com
//  
//  https://github.com/BrutPitt
//
//  https://michelemorrone.eu
//  https://BrutPitt.com
//
//  This software is distributed under the terms of the BSD 2-Clause license:
//  
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//      * Redistributions of source code must retain the above copyright
//        notice, this list of conditions and the following disclaimer.
//      * Redistributions in binary form must reproduce the above copyright
//        notice, this list of conditions and the following disclaimer in the
//        documentation and/or other materials provided with the distribution.
//   
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
//  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
//  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
////////////////////////////////////////////////////////////////////////////////
#version 120
//#pragma debug(on)

const float NEAR_POINT = 1.0/128.0;     // delta is used in the finite difference approximation of the gradient
                                    // (to determine normals)
const float DISTANCE = 5.0-2.0*NEAR_POINT;
const float CUBE_SIZE = 2.0;

const vec3 AMBIENT_LIGHT = vec3(.0, .0, .1);



vec3 gLight = vec3(1.0, 1.0, 0.1);

uniform sampler3D volumeTex;
uniform sampler2D backgroundTex;
uniform bool transparent;

float brightness = 2.0;
float density = .2;

varying vec4 ptOrigin;
varying vec4 ptDirection;
varying vec4 ptLight ;
varying vec4 ptView ;
varying vec4 screenPos;
varying vec4 ptVx;



bool IntersectBox(vec3 ptO, vec3 ptD, vec3 boxmin, vec3 boxmax, out float tnear, out float tfar)
{
    // compute intersection of ray with all six bbox planes
    vec3 invR = 1.0 / ptD;
    vec3 tbot = invR * (boxmin.xyz - ptO);
    vec3 ttop = invR * (boxmax.xyz - ptO);

    // re-order intersections to find smallest and largest on each axis
    vec3 tmin = min (ttop, tbot);
    vec3 tmax = max (ttop, tbot);

    // find the largest tmin and the smallest tmax
    vec2 t0 = max (tmin.xx, tmin.yz);
    float largest_tmin = max (t0.x, t0.y);
    t0 = min (tmax.xx, tmax.yz);
    float smallest_tmax = min (t0.x, t0.y);

    // check for hit
    bool hit;
    if ((largest_tmin > smallest_tmax)) 
        hit = false;
    else
        hit = true;

    tnear = largest_tmin;
    tfar = smallest_tmax;

    return hit;
}


/////////////////////////////////
// Refract Attributes and Functions
/////////////////////////////////
vec3 etaValues = vec3( 1.001, 1.101, 1.099  );
//vec3 fresnelValues = {2.0, 2.0, 0.1 };

vec3 refract(vec3 i, vec3 n, float eta)
{
    float cosi = dot(i, n);
    float cost2 = 1.0 - eta * eta * (1.0 - cosi*cosi);
    vec3 t = eta*i + ((eta*cosi - sqrt(abs(cost2))) * n);
    return t; //vec3(cost2 > 0.0 ? t : i);// t * vec3(cost2 > 0.0); 
}

// fresnel approximation
float fast_fresnel(vec3 I, vec3 N, vec3 fresnelValues)
{
    float power = fresnelValues.x;
    float scale = fresnelValues.y;
    float bias = fresnelValues.z;

    return bias + pow(1.0 - dot(I, N), power) * scale;
}


/////////////////////////////////
// Squared background
/////////////////////////////////

const float SQUARE_N = 20;
const float factor = 1.0/SQUARE_N;

vec3 squareBackground(vec2 coord)
{

    float x=coord.x;
    float y=coord.y;
    int xCol=0, yCol=0;

    while(x-factor>0) { x-=factor; xCol = bool(xCol) ? 0 : 1; }
    while(y-factor>0) { y-=factor; yCol = bool(yCol) ? 0 : 1; }

    return xCol+yCol==1 ? vec3(0.0,0.0,0.0) : vec3(0.0,0.0,1.0);

}



float Sq(vec3 p)
{
    return texture3D(volumeTex, p).x;
}


// ----------- normEstimate() -------------------------------------------------------
//
// Create a shading normal for the current point.  We use an approximate normal of
// the isosurface of the potential function, though there are other ways to
// generate a normal (e.g., from an isosurface of the potential function).
//
#define REFRACTION

vec3 normEstimateP(vec3 P)
{

   float gx1 = Sq( P - vec3( NEAR_POINT, 0, 0 ) );
   float gx2 = Sq( P + vec3( NEAR_POINT, 0, 0 ) );
   float gy1 = Sq( P - vec3( 0, NEAR_POINT, 0 ) );
   float gy2 = Sq( P + vec3( 0, NEAR_POINT, 0 ) );
   float gz1 = Sq( P - vec3( 0, 0, NEAR_POINT ) );
   float gz2 = Sq( P + vec3( 0, 0, NEAR_POINT ) );

   float gradX = gx2-gx1;
   float gradY = gy2-gy1;
   float gradZ = gz2-gz1;

   vec3 N = normalize(vec3( gradX, gradY, gradZ ));

   return N;
}


vec3 normEstimate(vec3 P)
{

   float gx1 = Sq( P - vec3( NEAR_POINT, 0, 0 ) );
   float gx2 = Sq( P + vec3( NEAR_POINT, 0, 0 ) );
   float gy1 = Sq( P - vec3( 0, NEAR_POINT, 0 ) );
   float gy2 = Sq( P + vec3( 0, NEAR_POINT, 0 ) );
   float gz1 = Sq( P - vec3( 0, 0, NEAR_POINT ) );
   float gz2 = Sq( P + vec3( 0, 0, NEAR_POINT ) );

   float gradX = gx2>gx1 ? gx2-gx1 : gx1-gx2;
   float gradY = gy2>gy1 ? gy2-gy1 : gy1-gy2;
   float gradZ = gz2>gz1 ? gz2-gz1 : gz1-gz2;

   vec3 N = normalize(vec3( gradX, gradY, gradZ ));

   return N;
}

// ----------- Phong() --------------------------------------------------
//
// Computes the direct illumination for point pt with normal N due to
// a point light at light and a viewer at eye.
//


const vec4 colorMaterial = vec4(.2, 0.7, 1.0, 1.0 );
const vec4 deepColor = colorMaterial * .33;

void Phong( vec3 light, vec3 eye, vec3 pt, vec3 N, out vec3 diffuse, out float specular )
{
   const int specularExponent = 10;             // shininess of shading
   const float specularity = 1.45;              // amplitude of specular highlight

   vec3 L     = normalize(-light);  // find the vector to the light
   vec3 E     = normalize( eye + pt );  // find the vector to the eye
   float  NdotL = dot( N, L );              // find the cosine of the angle between light and normal
   vec3 R     = L - 2 * NdotL * N;        // find the reflected vector

   //diffuse += abs( N )*0.3;  // add some of the normal to the
                             // color to make it more interesting



   // compute the illumination using the Phong equation
   diffuse = colorMaterial.xyz * max( NdotL, 0 );
   specular = specularity*pow( max(dot(E,R),0), specularExponent );

}

// fresnel approximation
float mfresnel(vec3 I, vec3 N, float power, float scale, float bias)
{
    return bias + (pow(clamp(1.0 - dot(I, N), 0.0, 1.0), power) * scale);
}

vec3 myrefract(vec3 i, vec3 n, float eta)
{
    float cosi = dot(i, n);
    float cost2 = 1.0 - eta * eta * (1.0 - cosi*cosi);
    vec3 t = eta*i + ((eta*cosi - sqrt(abs(cost2))) * n);
    return t; //vec3(cost2 > 0.0 ? t : i);// t * vec3(cost2 > 0.0); 
}

//float threshold = .999;
const int steps = 1000;
const float stepsize = CUBE_SIZE / float(steps);                                                                                     

//vec3 boxMin = vec3(-.2, -.2, -.2);
//vec3 boxMax = vec3(.2, .2, .2);
 
const vec3 boxMin = vec3(-1.0, -1.0, -1.) / DISTANCE;
const vec3 boxMax = vec3( 1.0,  1.0,  1.) / DISTANCE;

const float thresold = .55;
const float kRefract = .93;

void main(void)
{
    
    vec3 ptD = normalize(ptDirection.xyz);            //Direction    
    vec3 ptO = normalize(ptOrigin.xyz);               //Origin
    vec3 ptL = normalize(ptLight.xyz);                //Light
    vec3 ptW = normalize(screenPos.xyz);              //Screen
    vec3 ptV = normalize(ptVx.xyz);              //Screen

    //Get background color
    ptW.xy = (1+ptW.xy)*.5;
    vec4 c = vec4(texture2D(backgroundTex, ptW.xy).rgb, 0.0);//
    //vec4 c = vec4(0.0, 0.0, 0.0, 0.0);

    float tnear, tfar;

    bool hit = IntersectBox(ptO, ptD, boxMin, boxMax, tnear, tfar);
    //if(!hit) return;
    if(tnear < 0.0) tnear = 0.0;

    vec3 Pnear = ptO + (ptD * tnear);
    vec3 Pfar  = ptO + (ptD * tfar);

    Pnear = Pnear*(DISTANCE*.5) + 0.5;
    Pfar = Pfar*(DISTANCE*.5) + 0.5;


    vec3 diffuse;
    float specular;

    vec3 vRefract;
    
    // use front-to-back rendering
    vec3 Pstep = ptD * stepsize;
    vec3 P = Pnear;

    int accum =0;
    int i=0;
    float fresnel;
      
    for(; i<steps; i++) {        
        if(texture3D(volumeTex, P).x>thresold) {
            if(transparent) {
                vec3 N = normEstimate(P);
                //vec3 I = normalize(ptW);
                vRefract = refract(vec3(ptW.xy, float(i)/float(steps)) /**1.44+.5*/, N, kRefract);
                float fresnel = mfresnel(ptW, N, 2.0, 2.0, 0.1);
                //vec3 R = reflect(I*1.5 + 0.5, N);
                //float fresnel = mfresnel(-I, N, 5.0, .99, 0.01);
                //vec4 cR = vec4(texture2D(backgroundTex, R.xy).rgb, 0.0);
                N = normEstimateP(P);
                Phong(ptL, ptO, P, N, diffuse, specular);
            } else {
                vec3 N = normEstimateP(P);
                Phong(ptL, ptO, P, N, diffuse, specular);
                c = vec4(diffuse  + specular, 1.0);
            }
            accum++;
            break;
        }
//Increment ray from near to far
      P += Pstep;
    }

    float faceCount = 1;

    if(transparent && accum>0) {
        bool flagIN = false;
        //P=Pfar;
        //i+=5;

        for(; i<steps; i++) {        
            if(texture3D(volumeTex, P).x>thresold) {
                //vec3 I = normalize(ptW);
                if(!flagIN) {
                    vec3 N = normEstimate(P);
                    vRefract += refract(vec3(ptW.xy, float(i)/float(steps)), N, kRefract);
                    faceCount++;
                    flagIN = true;
                }
                //vec3 R = reflect(I*1.5 + 0.5, N);
                
                //vec4 cR = vec4(texture2D(backgroundTex, R.xy).rgb, 0.0);
                //Phong(ptL, ptO , P, N, diffuse, specular);
                
                accum++;
                //depth+= i;
                //break;
            } else {
                if(flagIN) {
                    vec3 N = normEstimate(P);
                    vRefract -= refract(vec3(ptW.xy, float(i)/float(steps)), N, kRefract);
                    //faceCount++;
                    flagIN = false;
                }
            }

          P += Pstep;
        }
    }



    if(transparent && accum>0) {
        vec4 cT = vec4(texture2D(backgroundTex, vRefract.xy/float(faceCount*2)).rgb, 0.0);
        float v = min(float(accum*2.0),float(steps-1))/float(steps);
        float v2 = v;
        vec4 opacityColor = mix(colorMaterial,deepColor,v2);
        gl_FragColor = /*vec4(diffuse,1.0)*.2 + */
                        //cT*.25 + 
                        mix(cT,opacityColor,clamp(log2(1.0+v), 0.0, .99)) +
                        specular*.75;
    }
    else
        gl_FragColor = c;
        
        //gl_FragColor = (c + mix(c,vec4(.2, 0.4, 1.0,1.0), float(accum*8)/float(steps)))*.5;
}


