# Cheatsheet — Value Categories

```
                        expression
                       ┌───────────┐
                       │  glvalue  │           rvalue
                  ┌────┴────┐                ┌────┴────┐
               lvalue     xvalue ────────────┘      prvalue
```

| Category | Identity | Movable | Examples |
|----------|----------|---------|----------|
| lvalue   | yes | no  | `x`, `*p`, `a[i]`, `++x`, `s.member`, string literal |
| xvalue   | yes | yes | `std::move(x)`, `static_cast<T&&>(x)`, function returning `T&&` |
| prvalue  | no  | yes | `42`, `a+b`, `Widget{}`, function returning `T` (by value) |

```
   glvalue = "has identity (a location)"   = lvalue OR xvalue
   rvalue  = "can be moved from"           = xvalue OR prvalue
```

## Key facts
- `std::move(x)` is **just a cast** to `T&&` (makes an xvalue). It moves nothing by itself.
- `std::forward<T>(x)` casts back to the original category — use **only** with `T&&` forwarding refs in templates.
- A named rvalue reference variable is itself an **lvalue**: `void f(T&& x){ /* x is an lvalue here! */ g(std::move(x)); }`
- Binding a prvalue/xvalue to `const T&` or `T&&` **extends its lifetime** to the reference's scope (with limits — see Module 16).
- C++17: prvalues are "unmaterialized" until needed → guaranteed copy elision.

## Reference binding table
| Initializer ↓ / Ref → | `T&` | `const T&` | `T&&` |
|---|---|---|---|
| lvalue `T` | ✅ | ✅ | ❌ |
| const lvalue | ❌ | ✅ | ❌ |
| rvalue (prvalue/xvalue) | ❌ | ✅ (extends) | ✅ (extends) |
