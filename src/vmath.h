#pragma once

#include <cmath>

constexpr float MATH_PI = static_cast<float>(M_PI);

float deg2rad(float angle) { return angle / 180 * MATH_PI; }
float rad2deg(float angle) { return angle / MATH_PI * 180; }

union vec2 {
    float data[2];
    struct {
        float x;
        float y;
    };
    vec2() : x(0), y(0) {}
    vec2(float x) : x(x), y(x) {}
    vec2(float x, float y) : x(x), y(y) {}
    vec2 add(vec2 v) {
        return vec2{x + v.x, y + v.y};
    }
    vec2 sub(vec2 v) {
        return vec2{x - v.x, y - v.y};
    }
    vec2 mul(float k) {
        return vec2{x * k, y * k};
    }
};

union vec3 {
    float data[3];
    struct {
        float x;
        float y;
        float z;
    };
    vec3() : x(0), y(0), z(0) {}
    vec3(float x) : x(x), y(x), z(x) {}
    vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    vec3(vec2 v, float z) : x(v.x), y(v.y), z(z) {}
    vec3 add(vec3 v) {
        return vec3{x + v.x, y + v.y, z + v.z};
    }
    vec3 sub(vec3 v) {
        return vec3{x - v.x, y - v.y, z - v.z};
    }
    vec3 mul(float k) {
        return vec3{x * k, y * k, z * k};
    }
    vec3 mul(vec3 v) {
        return vec3{x * v.x, y * v.y, z * v.z};
    }
    float dot(vec3 v) {
        return x * v.x + y * v.y + z * v.z;
    }
    vec3 cross(vec3 v) {
        return vec3{
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x};
    }
    float length() {
        return sqrtf(x * x + y * y + z * z);
    }
    vec3 normalized() {
        float k = 1 / length();
        return vec3{k * x, k * y, k * z};
    }
};

union vec4 {
    float data[4];
    struct {
        float x;
        float y;
        float z;
        float w;
    };
    vec4() : x(0), y(0), z(0), w(0) {}
    vec4(float x) : x(x), y(x), z(x), w(x) {}
    vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    vec4(vec3 v, float w) : x(v.x), y(v.y), z(v.z), w(w) {}
    float dot(vec4 v) {
        return x * v.x + y * v.y + z * v.z + w * v.w;
    }
    vec4 Mul(float k) {
        return vec4{x * k, y * k, z * k, w * k};
    }
};

union quat {
    float data[4];
    struct {
        float x;
        float y;
        float z;
        float w;
    };
    quat() : x(0), y(0), z(0), w(1) {}
    quat(float x) : x(x), y(x), z(x), w(x) {}
    quat(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    quat(vec3 v, float w) : x(v.x), y(v.y), z(v.z), w(w) {}
    quat fromAxisAngle(float x, float y, float z, float angle) {
        return fromAxisAngle(vec3{x, y, z}, angle);
    }
    quat fromAxisAngle(vec3 axis, float angle) {
        axis = axis.normalized();
        float a2 = deg2rad(angle) / 2;
        float sina2 = sinf(a2);
        return quat{
            axis.x * sina2,
            axis.y * sina2,
            axis.z * sina2,
            cosf(a2)};
    }
    quat combine(quat r) {
        return quat{
            r.w * x + r.x * w - r.y * z + r.z * y,
            r.w * y + r.x * z + r.y * w - r.z * x,
            r.w * z - r.x * y + r.y * x + r.z * w,
            r.w * w - r.x * x - r.y * y - r.z * z};
    }
    vec3 rotatedVector(vec3 v) {
        float ww = w * w;
        float wx = w * x;
        float wy = w * y;
        float wz = w * z;
        float xx = x * x;
        float xy = x * y;
        float xz = x * z;
        float yy = y * y;
        float yz = y * z;
        float zz = z * z;
        return vec3{
            (1 - 2 * yy - 2 * zz) * v.x + 2 * (xy + wz) * v.y + 2 * (xz - wy) * v.z,
            2 * (xy - wz) * v.x + (1 - 2 * xx - 2 * zz) * v.y + 2 * (yz + wx) * v.z,
            2 * (xz + wy) * v.x + 2 * (yz - wx) * v.y + (1 - 2 * xx - 2 * yy) * v.z};
    }
};

union mat3 {
    float data[9];
    vec3 columns[3];
    mat3() {
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 3; col++) {
                if (row == col) {
                    data[col * 3 + row] = 1;
                } else {
                    data[col * 3 + row] = 0;
                }
            }
        }
    }
};

union mat4 {
    float data[16];
    mat4() {
        for (int row = 0; row < 4; row++) {
            for (int col = 0; col < 4; col++) {
                if (row == col) {
                    data[col * 4 + row] = 1;
                } else {
                    data[col * 4 + row] = 0;
                }
            }
        }
    }
    float GetElementRowMajor(int index) {
        int row = index / 4;
        int col = index % 4;
        return data[4 * col + row];
    }
    float GetElementColumnMajor(int index) {
        return data[index];
    }
    float GetCell(int row, int col) {
        return data[col * 4 + row];
    }
    void SetCell(int row, int col, float value) {
        data[col * 4 + row] = value;
    }
    vec4 GetRow(int row) {
        return vec4{
            data[0 + row],
            data[4 + row],
            data[8 + row],
            data[12 + row]};
    }
    void SetRow(int row, vec4 value) {
        data[0 + row] = value.x;
        data[4 + row] = value.y;
        data[8 + row] = value.z;
        data[12 + row] = value.w;
    }
    vec4 GetColumn(int col) {
        return vec4{
            data[4 * col],
            data[4 * col + 1],
            data[4 * col + 2],
            data[4 * col + 3]};
    }
    void SetColumn(int col, vec4 value) {
        data[4 * col] = value.x;
        data[4 * col + 1] = value.y;
        data[4 * col + 2] = value.z;
        data[4 * col + 3] = value.w;
    }
    vec4 Mul(vec4 v) {
        vec4 r;
        r.x = GetRow(0).dot(v);
        r.y = GetRow(1).dot(v);
        r.z = GetRow(2).dot(v);
        r.w = GetRow(3).dot(v);
        return r;
    }
};

struct TTriangle {
    vec3 a;
    vec3 b;
    vec3 c;
};

mat4 btri(vec3 v0, vec3 v1, vec3 v2) {
    vec3 e1 = v1.sub(v0);
    vec3 e2 = v2.sub(v0);
    vec3 n = e1.cross(e2).normalized();
    float x1, x2, k;
    float num = v0.dot(n);
    mat4 transform{};
    if (fabs(n.x) > fabs(n.y) && fabs(n.x) > fabs(n.z)) {
        x1 = v1.y * v0.z - v1.z * v0.y;
        x2 = v2.y * v0.z - v2.z * v0.y;
        k = 1 / n.x;

        transform.SetColumn(0, vec4{0, 0, 1, 0});
        transform.SetColumn(1, vec4{e2.z, -e1.z, n.y, 0}.Mul(k));
        transform.SetColumn(2, vec4{-e2.y, e1.y, n.z, 0}.Mul(k));
    } else if (fabs(n.y) > fabs(n.z)) {
        x1 = v1.z * v0.x - v1.x * v0.z;
        x2 = v2.z * v0.x - v2.x * v0.z;
        k = 1 / n.y;

        transform.SetColumn(0, vec4{-e2.z, e1.z, n.x, 0}.Mul(k));
        transform.SetColumn(1, vec4{0, 0, 1, 0});
        transform.SetColumn(2, vec4{e2.x, -e1.x, n.z, 0}.Mul(k));
    } else if (fabs(n.z) > 0) {
        x1 = v1.x * v0.y - v1.y * v0.x;
        x2 = v2.x * v0.y - v2.y * v0.x;
        k = 1 / n.z;

        transform.SetColumn(0, vec4{e2.y, -e1.y, n.x, 0}.Mul(k));
        transform.SetColumn(1, vec4{-e2.x, e1.x, n.y, 0}.Mul(k));
        transform.SetColumn(2, vec4{0, 0, 1, 0});
    } else {
        throw "Degenerated triangle";
    }
    transform.SetColumn(3, vec4{x2 * k, -x1 * k, -num * k, 1});
    vec4 b = transform.Mul(vec4{v1, 1});
    transform.SetCell(3, 3, 1 / b.x);
    return transform;
}

mat4 btri(vec3 t[3]) {
    return btri(t[0], t[1], t[2]);
}

mat4 btri(TTriangle* t) {
    return btri(t->a, t->b, t->c);
}

mat4 btri2(TTriangle* tr) {
    vec3 e1 = tr->b.sub(tr->a);
    vec3 e2 = tr->c.sub(tr->a);
    vec3 n = e1.cross(e2).normalized();
    mat4 t{};
    vec3 f;
    if (fabs(n.x) > fabs(n.y) && fabs(n.x) > fabs(n.z)) {
        f.x = 1;
    } else if (fabs(n.y) > fabs(n.z)) {
        f.y = 1;
    } else if (fabs(n.z) > 0) {
        f.z = 1;
    }
    t.SetColumn(0, vec4{e1, 0});
    t.SetColumn(1, vec4{e2, 0});
    t.SetColumn(2, vec4{f, 0});
    t.SetColumn(0, vec4{tr->a, 1});
}

bool intersect(mat4 t, vec3 o, vec3 r, vec3* point, vec3* bc) {
    constexpr float tNear = 0.000001f;
    constexpr float tFar = 1000000.0f;
    const float transS = t.GetElementRowMajor(8) * o.x + t.GetElementRowMajor(9) * o.y + t.GetElementRowMajor(10) * o.z + t.GetElementRowMajor(11);
    const float transD = t.GetElementRowMajor(8) * r.x + t.GetElementRowMajor(9) * r.y + t.GetElementRowMajor(10) * r.z;
    const float ta = -transS / transD;
    if (ta <= tNear || ta >= tFar)
        return false;

    point->x = o.x + ta * r.x;
    point->y = o.y + ta * r.y;
    point->z = o.z + ta * r.z;

    // Calculate "x" and "y" barycentric coordinates

    const float k = t.GetCell(3, 3);

    bc->y = k * (t.GetElementRowMajor(0) * point->x + t.GetElementRowMajor(1) * point->y + t.GetElementRowMajor(2) * point->z + t.GetElementRowMajor(3));
    bc->z = k * (t.GetElementRowMajor(4) * point->x + t.GetElementRowMajor(5) * point->y + t.GetElementRowMajor(6) * point->z + t.GetElementRowMajor(7));
    bc->x = 1 - bc->y - bc->z;

    // final intersection test

    if (bc->x >= 0 && bc->y >= 0 && bc->z >= 0)
        return true;

    return false;
}

struct DiamondSphere {
    struct RegionVertex {
        float t;
        float r;
        float b;
        float l;
    };

    int subdivisionCount;
    float radius;
    void FindRegion(float latitude, float longitude, TTriangle* triangle, vec3* point) {
        auto y = latitude2level(latitude, subdivisionCount * 2);
        auto xt = longitude2level(longitude, y.t);
        auto xb = longitude2level(longitude, y.b);
        auto y1 = RegionVertex{
            .t = 90 - y.t / (subdivisionCount * 2) * 90,
            .b = 90 - y.b / (subdivisionCount * 2) * 90,
        };
        auto xt1 = RegionVertex{
            .l = xt.l / y.t * 90,
            .r = xt.r / y.t * 90,
            .b = xt.r / y.b * 90,
        };
        auto xb1 = RegionVertex{
            .l = xb.l / y.b * 90,
            .r = xb.r / y.b * 90,
            .t = xb.l / y.t * 90,
        };
        vec3 ray = spherical2cartesian(latitude, longitude, radius);
        triangle->a = spherical2cartesian(y1.b, xt1.b, radius);
        triangle->b = spherical2cartesian(y1.t, xt1.r, radius);
        triangle->c = spherical2cartesian(y1.t, xt1.l, radius);
        vec3 bc;
        mat4 m = btri(triangle);
        if (intersect(m, vec3{}, ray, point, &bc))
            return;
        triangle->a = spherical2cartesian(y1.b, xb1.l, radius);
        triangle->b = spherical2cartesian(y1.b, xb1.r, radius);
        triangle->c = spherical2cartesian(y1.t, xb1.t, radius);
        m = btri(triangle);
        if (intersect(m, vec3{}, ray, point, &bc))
            return;
        throw "";
    }
    RegionVertex longitude2level(float longitude, int parallelLen) {
        float x = floor(longitude / 180.0 * parallelLen * 2);
        return RegionVertex{.l = x, .r = x + 1};
    }
    RegionVertex latitude2level(float latitude, int meridianLen) {
        float x = floor((90.0 - latitude) / 90.0 * meridianLen);
        return RegionVertex{.t = x, .b = x + 1};
    }
    vec3 spherical2cartesian(float latitude, float longitude, float altitude) {
        float theta = deg2rad(longitude);
        float phi = deg2rad(90 - latitude);
        vec3 result;
        result.z = altitude * cos(theta) * sin(phi);
        result.x = altitude * sin(theta) * sin(phi);
        result.y = altitude * cos(phi);
        return result;
    }
};
