#include <math.h>
#define RADIO_TIERRA 4182.44949

double aRadianes(double num){
    double rad = 0.0;
    rad = M_PI * (num/180.0);
    return rad;
}

double haversine(float num){
    return pow(sin(num/2.0),2);
}

double calcularDistancia(double lon1, double lat1, double lon2, double lat2){
    double distancia = 0.0;

    double deltaLat = aRadianes(lat1 - lat2);
    double deltaLon = aRadianes(lon1 - lon2);

    double a = haversine(deltaLat) + cos(aRadianes(lat1)) * cos(aRadianes(lat2)) * haversine(deltaLon);
    
    double c = 2.0 * atan2(sqrt(a),sqrt(1-a));

    distancia = RADIO_TIERRA * c;

    return distancia;
}

