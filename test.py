n, m = map(int, input().split())
v = [0 for _ in range(n + 1)]
v[1: n + 1] = list(map(int, input().split()))

tag = [0 for _ in range(4 * n + 1)]
sum = [0 for _ in range(4 * n + 1)]

def build(now, l, r):
    tag[now] = 0
    if l == r:
        sum[now] = v[l]
        return
    mid = (l + r) >> 1
    build(now << 1, l, mid)
    build(now << 1 | 1, mid + 1, r)
    sum[now] = sum[now << 1] + sum[now << 1 | 1]

def pushdown(now, l, r):
    ls, rs = now << 1, now << 1|1
    if tag[now] != 0:
        mid = (l + r) >> 1
        tag[ls] += tag[now]
        tag[rs] += tag[now]
        sum[ls] += (mid + 1 - l) * tag[now]
        sum[rs] += (r - mid) * tag[now]
        tag[now] = 0

def query(now, l, r, L, R):
    if L <= l and r <= R:
        return sum[now]
    if r < L or l > R:
        return 0
    ls, rs = now << 1, now << 1 | 1
    mid = (l + r) >> 1
    pushdown(now, l, r)
    return query(ls, l, mid, L, R) + query(rs, mid + 1, r, L, R)

def modi(now, l, r, L, R, d):
    if L <= l and r <= R:
        sum[now] += (r + 1 - l) * d 
        tag[now] += d
        return
    if r < L or l > R:
        return
    ls, rs = now << 1, now << 1 | 1
    mid = (l + r) >> 1
    pushdown(now, l, r)
    modi(ls, l, mid, L, R, d)
    modi(rs, mid + 1, r, L, R, d)
    sum[now] = sum[ls] + sum[rs]

build(1, 1, n)

for _ in range(m):
    opt = list(map(int, input().split()))
    if opt[0] == 1:
        modi(1, 1, n, opt[1], opt[2], opt[3])
    else:
        print(query(1, 1, n, opt[1], opt[2]))