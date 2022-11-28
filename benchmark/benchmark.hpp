#pragma once



namespace jada{

double fRand(double fMin, double fMax)
{
    double f = static_cast<double>(rand()) / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

template<class C>
void assign_random(C& c){

    for (auto& e : c){
        e = fRand(-32.8, 4532.3245);
    }


}

struct d_CD4 {

    static constexpr size_t padding = 2;

    d_CD4(double delta) : m_delta(delta) {}

    template <class F> auto operator()(F f) const { 
       return (-f(+2) + 8.0 * f(+1) - 8.0 * f(-1) + f(-2)) / m_delta;
    }

private:
    double m_delta;

};



}