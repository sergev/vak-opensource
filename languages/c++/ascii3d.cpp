//
// For details, see https://gist.github.com/Wunkolo/249646f7a922ee045c70
//
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <algorithm>
#include <string>

typedef double Real;

union Vec3
{
    Vec3(Real X, Real Y, Real Z)
        :
        X(X), Y(Y), Z(Z)
    {
    }
    Real Reals[3];
    struct
    {
        Real X, Y, Z;
    };
    Real Length() const
    {
        return static_cast<Real>(
            sqrt(X*X + Y*Y + Z*Z)
            );
    }
    Vec3 Normalized() const
    {
        const Real Len = Length();
        return (*this) / Len;
    }

    Real Dot(const Vec3& Other) const
    {
        return X * Other.X + Y * Other.Y + Z * Other.Z;
    }

    Vec3 Abs() const
    {
        return Vec3{
            fabs(X),
            fabs(Y),
            fabs(Z)
        };
    }

    Vec3 operator + (const Vec3& Other) const
    {
        return Vec3{
            X + Other.X,
            Y + Other.Y,
            Z + Other.Z
        };
    }
    Vec3 operator - (const Vec3& Other) const
    {
        return Vec3{
            X - Other.X,
            Y - Other.Y,
            Z - Other.Z
        };
    }

    Vec3 operator * (const Vec3& Other) const
    {
        return Vec3{
            X * Other.X,
            Y * Other.Y,
            Z * Other.Z
        };
    }

    Vec3 operator / (const Vec3& Other) const
    {
        return Vec3{
            X / Other.X,
            Y / Other.Y,
            Z / Other.Z
        };
    }

    Vec3 operator % (const Vec3& Other) const
    {
        return Vec3{
            fmod(X,Other.X),
            fmod(Y,Other.Y),
            fmod(Z,Other.Z)
        };
    }

    Vec3 operator + (const Real& Other) const
    {
        return Vec3{
            X + Other,
            Y + Other,
            Z + Other
        };
    }

    Vec3 operator - (const Real& Other) const
    {
        return Vec3{
            X - Other,
            Y - Other,
            Z - Other
        };
    }

    Vec3 operator * (const Real& Other) const
    {
        return Vec3{
            X * Other,
            Y * Other,
            Z * Other
        };
    }

    Vec3 operator / (const Real& Other) const
    {
        return Vec3{
            X / Other,
            Y / Other,
            Z / Other
        };
    }

    Vec3 operator % (const Real& Other) const
    {
        return Vec3{
            fmod(X,Other),
            fmod(Y , Other),
            fmod(Z , Other)
        };
    }
};

namespace Shapes
{
    Real Plane(const Vec3& Position, const Vec3& Normal)
    {
        return Position.Dot(Normal.Normalized());
    }
    Real Sphere(const Vec3& Position, Real Radius)
    {
        return Position.Length() - Radius;
    }
    Real Box(const Vec3& Position, Vec3 Bounds)
    {
        Vec3 Dist = Position.Abs() - Bounds;
        return std::min<Real>(std::max(Dist.X, std::max<Real>(Dist.Y, Dist.Z)), 0.0) +
            Vec3{ std::max<Real>(Dist.X,0.0),
            std::max<Real>(Dist.Y,0.0),
            std::max<Real>(Dist.Z,0.0) }.Length();
    }
    Real RoundBox(const Vec3& Position, Vec3 Bounds, Real Radius)
    {
        return Box(Position, Bounds) - Radius;
    }
    Real Capsule(const Vec3& Position, Vec3 A, Vec3 B, Real Radius)
    {
        Vec3 Pa = Position - A;
        Vec3 Ba = B - A;
        Real H = (Pa.Dot(Ba) / Ba.Dot(Ba));
        H = std::min<Real>(0, std::max<Real>(1.0, H));
        return (Pa - (Ba*H)).Length() - Radius;
    }
    Real Torus(const Vec3& Position, Real InRadius, Real OutRadius)
    {
        Vec3 Q{ Vec3{ Position.X,Position.Z,0 }.Length() - OutRadius,Position.Y,0 };
        return Q.Length() - InRadius;
    }
}

namespace Operations
{
    Real Clamp(Real A, Real Min, Real Max)
    {
        const Real T = A < Min ? Min : A;
        return T > Max ? Max : T;
    }
    Real Lerp(Real A, Real B, Real T)
    {
        return fma(T, B, fma(-T, A, A));
    }
    Real Union(Real A, Real B)
    {
        return std::min(A, B);
    }
    Real SmoothUnion(Real A, Real B, Real K = 1)
    {
        /*
        Power Smooth
        A = std::pow(A, K);
        B = std::pow(B, K);
        return std::pow((A*B) / (A + B), Real(1) / K);
        */
        Real H = Clamp(Real(0.5) + Real(0.5)*(B - A) / K, 0, 1);
        return Lerp(B, A, H) - (K*H*(Real(1.0) - H));
    }
    Real Intersection(Real A, Real B)
    {
        return std::max(A, B);
    }
    Real Subtract(Real A, Real B)
    {
        return Intersection(-A, B);
    }
}

namespace Translate
{
    const static Real Pi(static_cast<Real>(3.1415926535897932384626433));
    Vec3 RotX(const Vec3& Position, Real Angle)
    {
        Angle *= Pi / Real(180);
        return Vec3{
            Position.X,
            Position.Y * cos(Angle) - Position.Z * sin(Angle),
            Position.Y * sin(Angle) + Position.Z * cos(Angle),
        };
    }
    Vec3 RotY(const Vec3& Position, Real Angle)
    {
        Angle *= Pi / Real(180);
        return Vec3{
            Position.Z * sin(Angle) + Position.X * cos(Angle),
            Position.Y,
            Position.Z * cos(Angle) - Position.X * sin(Angle),
        };
    }
    Vec3 RotZ(const Vec3& Position, Real Angle)
    {
        Angle *= Pi / Real(180);
        return Vec3{
            Position.X * cos(Angle) - Position.Y * sin(Angle),
            Position.X * sin(Angle) + Position.Y * cos(Angle),
            Position.Z,
        };
    }
    Vec3 Repeat(const Vec3& Position, const Vec3& Bounds)
    {
        return (Position % Bounds) - (Bounds*Real(0.5));
    }
    Vec3 RepeatGround(const Vec3& Position, const Vec3& Bounds)
    {
        Vec3 Rep{ Position };
        Rep.X = remainder(Rep.X, Bounds.X) - (Bounds.X * Real(0.5));
        Rep.Z = remainder(Rep.Z, Bounds.Z) - (Bounds.Z * Real(0.5));
        return Rep;
    }
}

#define WIDTH (110)
#define HEIGHT (50)
#define PREC 0.002

static size_t Tick = 0;

//// SCENE
Real Scene(const Vec3& Point)
{
    Real Distance = 0;

    Distance = Shapes::Plane(Point, Vec3{ 0,1,0 });

    Distance = Operations::Union(
        Distance,
        Shapes::RoundBox(
            Translate::RotZ(
                Point - Vec3{ -5,2,5 }, Real(15 + Tick * 15)
                ),
            Vec3{ 1,2,1 }, 0.525f)
        );

    Distance = Operations::Union(
        Distance,
        Shapes::Capsule(Point,
                        Vec3{ 5,5,6 },
                        Vec3{ -5,5,6 },
                        Real(3.75)
                        ));

    Distance = Operations::SmoothUnion(
        Distance,
        Shapes::Torus(
            Translate::RotX(Point - Vec3{ 0,Real(0.5),12 }, 45),
            Real(0.5), Real(7)
            ), 3
        );

    for( size_t i = 1; i < 5; i++ )
    {
        Distance = Operations::SmoothUnion(
            Distance,
            Shapes::Torus(
                Translate::RotZ(
                    Translate::RotX(Point - Vec3{ 0,Real(0.5),Real(12 + i * 7) }, 85), Real(i * 30 + (Tick * 5))
                    ),
                Real(1), Real(6)
                ), 3);
    }

    return Distance;
}

Vec3 CalcNormal(const Vec3& Point)
{
#define EPSILON static_cast<Real>(0.001)
    return Vec3{
        Scene(Point + Vec3{ EPSILON,0,0 }) - Scene(Point - Vec3{ EPSILON,0,0 }),
        Scene(Point + Vec3{ 0,EPSILON,0 }) - Scene(Point - Vec3{ 0,EPSILON,0 }),
        Scene(Point + Vec3{ 0,0,EPSILON }) - Scene(Point - Vec3{ 0,0,EPSILON }),
    }.Normalized();
}

Real March(const Vec3& Origin, const Vec3& Ray, bool* Hit)
{
    Real Distance = 0;
    for( size_t i = 0; i < 128; i++ )
    {
        Real ClosestSurface = Scene(Origin + (Ray * Distance));
        if( ClosestSurface < PREC )
        {
            // "Hit" a surface
            if( Hit != nullptr )
            {
                *Hit = true;
            }
            break;
        }
        Distance += ClosestSurface * Real(0.75);
    }
    return Distance;
}

Real Shadow(const Vec3& LightPos, const Vec3& LightDir, Real Min, Real Max, Real K)
{
    Real Res = 1;
    for( Real t = Min; t < Max;)
    {
        Real Distance = Scene(LightPos + LightDir*t);
        if( Distance < PREC )
        {
            return 0.0;
        }
        Res = std::min(Res, K*Distance / t);
        t += Distance;
    }
    return Res;
}

const char Shades[] = ".:*oe&#%@";

int main()
{
    Vec3 LightDir = Vec3{ 1,1,1 }.Normalized();
    Vec3 EyePos = Vec3{ 0,2,-6 };

    std::string Screen;
    Screen.reserve(WIDTH * HEIGHT);
    do
    {
        EyePos.Z += Real(0.25);
        Tick++;
        for( size_t y = 0; y < HEIGHT; y++ )
        {
            for( size_t x = 0; x < WIDTH; x++ )
            {
                Vec3 UV{
                    static_cast<Real>(x),
                    static_cast<Real>(y),
                    1 };
                UV = UV / Vec3{
                    WIDTH,
                    HEIGHT,
                    1 };

                // Recanonicalize [-1,1]
                UV = (UV * 2.0) - 1.0;

                // Flip Y axis
                UV.Y *= Real(-1);

                // Aspect Ratio Correction
                UV.X *= WIDTH / HEIGHT;
                //UV.X *= 0.75;

                /* Fov
                15deg bisec = tan((pi/12)/2)    = 30 degrees fov = 0.1316524975873958
                30deg bisec = tan((pi/6)/2)     = 60 degrees fov = 0.26794919243112270
                36deg bisec = tan(((2pi)/5)/2)  = 72 degrees fov = 0.7265425280053608858
                45dec bisec = tan((pi/2)/2)     = 90 degrees fov = 1
                */
                UV.Z = Real(1 / 0.7265425280053608858);
                UV = UV.Normalized();

                bool Hit = false;

                Real Distance = March(EyePos, UV, &Hit);

                Vec3 Point = EyePos + (UV * Distance);

                if( Hit )
                {
                    Vec3 Normal = CalcNormal(Point);
                    Real Diffuse = Normal.Dot(LightDir);
                    Diffuse *= 0.5;
                    Diffuse += 0.5;

                    Diffuse *= Diffuse;

                    Diffuse *= Shadow(Point, LightDir, Real(0.5), 10, 10);

                    Screen += Shades[static_cast<size_t>(Diffuse*(sizeof(Shades) - 2))];
                }
                else
                {
                    Screen += ' ';
                }
            }
            Screen += "\n";
        }
        printf("%s", Screen.c_str());

        Screen.clear();
    } while( Tick < 300 );
    return 0;
}
