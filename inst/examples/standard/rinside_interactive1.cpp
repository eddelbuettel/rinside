// -*- mode: C++; c-indent-level: 4; c-basic-offset: 4;  tab-width: 8; -*-
//
// Simple example showing how to do the standard 'hello, world' using embedded R
//
// Copyright (C) 2009 Dirk Eddelbuettel 
// Copyright (C) 2010 Dirk Eddelbuettel and Romain Francois
//
// GPL'ed 

#include <RInside.h>                    // for the embedded R via RInside

class Wrapper;

// A planet.
struct Planet {
    double x,y;
    double m;
    double vx, vy;
};

// A gravity simulator
class Solver {
    typedef std::vector<Planet> Planets;
    Planets tab;
    double dt;
    double G;
    void Iteration() {
        for(Planets::iterator a=tab.begin(); a != tab.end(); a++) {
            for(Planets::iterator b=tab.begin(); b != a; b++) {
                double x = a->x - b->x;
                double y = a->y - b->y;
                double r = sqrt(x*x + y*y);
                double f = a->m * b->m * G / (r*r+1);
                double fx = f * x/r;
                double fy = f * y/r;
                a->vx -= dt * fx / a->m;
                a->vy -= dt * fy / a->m;
                b->vx += dt * fx / b->m;
                b->vy += dt * fy / b->m;
            }
        }
        for(Planets::iterator a=tab.begin(); a != tab.end(); a++) {
            a->x += dt * a->vx;
            a->y += dt * a->vy;
        }
    }
public:
    Solver(int n): tab(n), dt(1.0e-4), G(1.0) {
        double v=0;
        for(Planets::iterator a=tab.begin(); a != tab.end(); a++) {
            a->x = sin(v);
            a->y = cos(v);
            a->m = 1;
            v += 3.0/n;
        }
    }
    void Iterate(int n) {
        for (int i=0;i<n;i++) Iteration();
    }
    friend class Wrapper;
};

// Exporting _ to make things more readable
using Rcpp::_;

// A nice wrapper for the solver
class Wrapper {
    Solver * s;
public:
    Wrapper(Solver * s_) : s(s_) {}
    Rcpp::DataFrame getData() {
        Rcpp::NumericVector x,y,m,vx,vy;
        for (Solver::Planets::iterator a=s->tab.begin(); a != s->tab.end(); a++) {
            x.push_back( a->x);            
            y.push_back( a->y);            
            m.push_back( a->m);            
            vx.push_back(a->vx);            
            vy.push_back(a->vy);            
        }
        return Rcpp::DataFrame::create(_["x"] = x,_["y"] = y,_["mass"] = m,_["Vx"] = vx,_["Vy"] = vy);
    }
    void setData(Rcpp::DataFrame tab) {
        if ((size_t)tab.nrows() != s->tab.size()) {
            return;
        }
        Rcpp::NumericVector x = tab["x"];
        Rcpp::NumericVector y = tab["y"];
        Rcpp::NumericVector m = tab["mass"];
        Rcpp::NumericVector vx = tab["Vy"];
        Rcpp::NumericVector vy = tab["Vy"];
        for (int i=0;i<tab.nrows();i++) {
            s->tab[i].x  = x[i];
            s->tab[i].y  = y[i];
            s->tab[i].m  = m[i];
            s->tab[i].vx = vx[i];
            s->tab[i].vy = vy[i];
        }
    }
    double& G() {
        return s->G;
    }
    double& dt() {
        return s->dt;
    }
};

// The function which is called when running Solver$... 
SEXP Dollar(Rcpp::XPtr<Wrapper> obj, std::string name) {
    if (name == "data") {
        return obj->getData();
    } else if (name == "G") {
        return Rcpp::NumericVector(1,obj->G());
    } else if (name == "dt") {
        return Rcpp::NumericVector(1,obj->dt());
    } else {
        return NULL;
    }
}

// The function which is called when assigning to Solver$... 
Rcpp::XPtr<Wrapper> DollarAssign(Rcpp::XPtr<Wrapper> obj, std::string name, SEXP v) {
    if (name == "data") {
        obj->setData(v);
    } else if (name == "G") {
        obj->G() = Rcpp::NumericVector(v)[0];
    } else if (name == "dt") {
        obj->dt() = Rcpp::NumericVector(v)[0];
    }
    return obj;
}

// The function listing the elements of Solver
Rcpp::CharacterVector Names(Rcpp::XPtr<Wrapper> obj) {
    Rcpp::CharacterVector ret;
    ret.push_back("data");
    ret.push_back("G");
    ret.push_back("dt");
    return ret;
}

int main(int argc, char *argv[]) {
    Solver S(70); // Creating the gravity simulator
    
    RInside R(argc, argv, false, false, true); // Create an embedded R instance 
    Rcpp::XPtr<Wrapper> wr(new Wrapper(&S));   // Wrapping the solver
    wr.attr("class") = "Solver";
    R["Solver"] = wr; // Adding the wrapped solver
    R["$.Solver"] = Rcpp::InternalFunction(& Dollar); // Adding the functions
    R["$<-.Solver"] = Rcpp::InternalFunction(& DollarAssign);
    R["names.Solver"] = Rcpp::InternalFunction(& Names);
    char type;
    do {
        std::cout << "Want to go interactive? [y/n]";
        std::cin  >> type;
    } while ( !std::cin.fail() && type!='y' && type!='n' );
    if (type == 'y') { // Running an interactive R session
        std::cout << "Running an interactive R session. You can explore (and modify) the data in the 'Solver' object." << std::endl;
        std::cout << "[ You can finish the with Ctrl+D ]" << std::endl;
        R.parseEval("options(prompt = 'R console > ')");
        R.parseEval("X11()");
        R.repl() ;
        std::cout << "Finishing the interactive mode" << std::endl;
        R.parseEval("dev.off()");
    }
    R.parseEval("X11()");
    for (int i=0;i<2000;i++) { // Running the some 200'000 iterations in non-interactive mode
        S.Iterate(100);
        R.parseEval("P = Solver$data;");
        R.parseEval("plot(P$x,P$y,cex=P$mass,asp=1,xlim=c(-5,5),ylim=c(-5,5),xlab='X',ylab='Y')");
    }
    exit(0);
}

