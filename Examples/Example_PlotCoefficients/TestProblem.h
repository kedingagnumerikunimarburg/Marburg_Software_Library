#include <string>

/**
 * TestProblem
 * Different test problems with homogeneous Dirichlet b.c.'s on [0,1]
 *
 *  N =
 *      1: y(t) = x*(1-x),  -y''(t) = 2
 *      2:   y(t) = exp(-100*(x-0.5)^2),
 *        -y''(t) = (200-(200x-100)^2)*exp(-100*(x-0.5)^2)
 *      3: 1D example from [BBCCDDU]
 */
template <unsigned int N>
class TestProblem : public SimpleSturmBVP
{
    public:
        double p(const double t) const
        {
            switch(N)
            {
                case 1:
                    return 1;
                    break;
                case 2:
                    return 1;
                    break;
                case 3:
                    return 1;
                    break;
                default:
                    return 0;
                    break;
            }
        }

        double p_prime(const double t) const
        {
            switch(N)
            {
                case 1:
                    return 0;
                    break;
                case 2:
                    return 0;
                    break;
                case 3:
                    return 0;
                    break;
                default:
                    return 0;
                    break;
            }
        }

        double q(const double t) const
        {
            switch(N)
            {
                case 1:
                    return 0;
                    break;
                case 2:
                    return 0;
                    break;
                case 3:
                    return 0;
                    break;
                default:
                    return 0;
                    break;
            }
        }

        double g(const double t) const
        {
            switch(N)
            {
                case 1:
                    return 2;
                    break;
                case 2:
                    return ( (200.0-(200.0*t-100.0)*(200.0*t-100.0)) *
                                            exp(-100.0*(t-0.5)*(t-0.5)) );
                    break;
                case 3:
                    return ( -100*exp(5*t)*(1-(exp(5*t)-1)/(exp(5.)-1)) /
                             (exp(5.)-1)+200*exp(10*t)/((exp(5.)-1) *
                             (exp(5.)-1))+100*(exp(5*t)-1)*exp(5*t) /
                             ((exp(5.)-1)*(exp(5.)-1)) );
                default:
                    return 0;
                    break;
            }
        }

        string toString() const
        {
            switch(N)
            {
                case 1:
                    return "y(t) = x*(1-x), -y''(t) = 2, y(0) = y(1) = 0";
                    break;
                case 2:
                    return "y(t) = exp(-100*(x-0.5)^2), -y''(t) = (200-(200x-100)^2)*exp(-100*(x-0.5)^2), y(0) = y(1) = 0";
                    break;
                case 3:
                    return "1D example from [BBCCDDU]";
                    break;
                default:
                    return "TestProblem: N not defined.";
                    break;
            }
        }

  bool bc_left() const { return true; }   /* boundary conditions left: true */
  bool bc_right() const { return true; }  /* boundary conditions right: true */
};
