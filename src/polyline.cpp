#include <Rcpp.h>
#include "googlePolylines.h"

using namespace Rcpp;

// [[Rcpp::export]]
DataFrame rcpp_decode_polyline(std::string encoded){
  int len = encoded.size();
  int index = 0;
  float lat = 0;
  float lng = 0;
  
  Rcpp::NumericVector pointsLat;
  Rcpp::NumericVector pointsLon;
  
  while (index < len){
    char b;
    int shift = 0;
    int result = 0;
    do {
      b = encoded.at(index++) - 63;
      result |= (b & 0x1f) << shift;
      shift += 5;
    } while (b >= 0x20);
    float dlat = ((result & 1) ? ~(result >> 1) : (result >> 1));
    lat += dlat;
    
    shift = 0;
    result = 0;
    do {
      b = encoded.at(index++) - 63;
      result |= (b & 0x1f) << shift;
      shift += 5;
    } while (b >= 0x20);
    float dlng = ((result & 1) ? ~(result >> 1) : (result >> 1));
    lng += dlng;
    
    pointsLat.push_back(lat * (float)1e-5);
    pointsLon.push_back(lng * (float)1e-5);
  }
  
  return DataFrame::create(Named("lat") = pointsLat, Named("lon") = pointsLon);
}

Rcpp::String EncodeNumber(int num){
  
  std::string out_str;
  
  while(num >= 0x20){
    out_str += (char)(0x20 | (int)(num & 0x1f)) + 63;
    num >>= 5;
  }
  
  out_str += char(num + 63);
  return out_str;
}

Rcpp::String EncodeSignedNumber(int num){
  
  int sgn_num = num << 1;
  
  if (sgn_num < 0) {
    sgn_num = ~sgn_num;
  }
  
  return EncodeNumber(sgn_num);
}

Rcpp::String encode_polyline(Rcpp::NumericVector latitude,
                             Rcpp::NumericVector longitude,
                             int num_coords){
  
  int plat = 0;
  int plon = 0;
  
  Rcpp::String output_str;
  
  for(int i = 0; i < num_coords; i++){
    
    int late5 = latitude[i] * 1e5;
    int lone5 = longitude[i] * 1e5;
    
    output_str += EncodeSignedNumber(late5 - plat);
    output_str += EncodeSignedNumber(lone5 - plon);
    
    plat = late5;
    plon = lone5;
  }
  return output_str;
}

// [[Rcpp::export]]
Rcpp::String rcpp_encode_polyline(Rcpp::NumericVector latitude,
                            Rcpp::NumericVector longitude,
                            int num_coords){
  
  Rcpp::String output_str = encode_polyline(latitude, longitude, num_coords);
  return output_str;
}

