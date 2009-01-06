%module linboxfunctions
%{
	#include <iostream>

extern bool det(std::istream& matrix_in, std::ostream& det_out);
extern const char* detFiles(char *matfile);
extern bool rank(std::istream& matrix_in, std::ostream& rank_out);
extern const char* rankFiles(char *matfile);
extern int estimateRankTime(char *matfile);
extern bool val(std::istream& matrix_in, std::ostream& val_out);
extern const char* valFiles(char *matfile);
extern bool trace(std::istream& matrix_in, std::ostream& trace_out);
extern const char* traceFiles(char *matfile);
extern bool smithNormalForm(std::istream& matrix_in, std::ostream& snf_out);
extern const char* smithNormalFormFiles(char *matfile);
%}

extern bool det(std::istream& matrix_in, std::ostream& det_out);
extern const char* detFiles(char *matfile);
extern bool rank(std::istream& matrix_in, std::ostream& rank_out);
extern const char* rankFiles(char *matfile);
extern int estimateRankTime(char *matfile);
extern bool val(std::istream& matrix_in, std::ostream& trace_out);
extern const char* valFiles(char *matfile);
extern bool trace(std::istream& matrix_in, std::ostream& trace_out);
extern const char* traceFiles(char *matfile);
extern bool smithNormalForm(std::istream& matrix_in, std::ostream& snf_out);
extern const char* smithNormalFormFiles(char *matfile);
