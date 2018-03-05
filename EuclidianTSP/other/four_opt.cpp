
/*
 vector<int> fourOpt(const vector<int>& cities, const vector<Point>& ps) {
 Tour tour(cities);
 bool start_again;
 do {
 start_again = false;
 int a1 = 0, a2 = tour.next(a1), b1, b2, c1, c2, q1, q2;
 while (a1 != tour.prev(tour.prev(tour.prev(0)))) {
 double d_a1_a2 = ps[a1].distance(ps[a2]);
 b1 = tour.next(a2);
 b2 = tour.next(b1);
 while (b1 != tour.prev(tour.prev(0))) {
 double d_b1_b2 = ps[b1].distance(ps[b2]),
 c1 = tour.next(b2);
 c2 = tour.next(c1);
 while (c1 != tour.prev(0)) {
 double d_c1_c2 = ps[c1].distance(ps[c2]),
 d_a1_c2 = ps[a1].distance(ps[c2]),
 d_a2_c1 = ps[a2].distance(ps[c1]);
 q1 = tour.next(c2);
 q2 = tour.next(q1);
 while (q1 != 0) {
 double d_q1_q2 = ps[q1].distance(ps[q2]),
 d_b1_q2 = ps[b1].distance(ps[q2]),
 d_b2_q1 = ps[b2].distance(ps[q1]);
 if (d_a1_a2 + d_b1_b2 + d_c1_c2 + d_q1_q2 > d_a1_c2 + d_a2_c1 + d_b1_q2 + d_b2_q1) {
 cout << "before: " << totalTravellingDistance(tour.cities, ps) << "\n";
 cout.flush();
 tour.flip(q2, a1);
 tour.flip(b2, q2);
 tour.flip(q1, c2);
 tour.flip(c1, b2);
 cout << "after: " << totalTravellingDistance(tour.cities, ps) << "\n";
 start_again = true;
 break;
 }
 q1 = q2;
 q2 = tour.next(q1);
 }
 if (start_again) break;
 c1 = c2;
 c2 = tour.next(c1);
 }
 if (start_again) break;
 b1 = b2;
 b2 = tour.next(b1);
 }
 if (start_again) break;
 a1 = a2;
 a2 = tour.next(a1);
 }
 } while (start_again);
 return tour.cities;
 
 }
 */
