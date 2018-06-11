#pragma once

class GC_SA : GC {

    struct Coloring {
        std::vector<Color> colors;
        Count violations;
        Count color_count;
    };

    struct Derivative {
        Index node;
        Color new_color;
        Count new_violations;
    };

    const Graph* gr;
    std::uniform_int_distribution<> node_distr;
    std::default_random_engine rng;
    const Count iterations;

public:
    GC_SA(Count iterations) : iterations(iterations) {}

    ColoredGraph solve(const Graph& gr) override {
        this->gr = &gr;
        node_distr = std::uniform_int_distribution(0, gr.nodeCount()-1);

        GC_Naive naive;
        auto res = naive.solve(gr);

        Coloring coloring;
        coloring.violations = 0;
        coloring.color_count = res.colorCount();
        coloring.colors.resize(res.nodeCount());
        for (auto i = 0; i < res.nodeCount(); ++i) {
            coloring.colors[i] = res.color(i);
        }

        while (auto res = Solve(gr, iterations, coloring.color_count-1)) {
            coloring = res.value();
        }

        return {gr, coloring.colors};
    }

    string name() {
        return "SA";
    }

private:

    std::optional<Coloring> Solve(const Graph& gr, Count iterations, Count color_count) {
        std::uniform_real_distribution<> zero_one_distr;

        auto coloring = RandomColoring(gr.nodeCount(), color_count);
        auto trials = 0;
        auto beta = 0.98;
        for (auto iter = 0; iter < iterations; ++iter) {
            auto deriv = GenDerivative(coloring);
            double prob = 0;
            double t = 0;
            if (deriv.new_violations < coloring.violations ||
                // maybe change sign
                (prob = exp(t = (deriv.new_violations - coloring.violations)*beta)) > zero_one_distr(rng))  {

                //(prob = exp(t = (-(deriv.new_violations-coloring.violations)*beta))) > zero_one_distr(rng)) {
                Apply(coloring, deriv);
            }

            if (coloring.violations == 0) {
                return {coloring};
            }

            if (++trials == 3.4 * gr.nodeCount())
            {
                beta *= (0.2 + gr.nodeCount() / iterations) / 0.2;
                trials = 0;
            }
        }

        return {};
    }

    Derivative GenDerivative(const Coloring& coloring) {
        Node n = node_distr(rng);
        Color c = coloring.colors[n];
        std::uniform_int_distribution color_distr(0, coloring.color_count-1);

        auto new_c = c;
        while ((new_c = color_distr(rng)) == c);

        auto new_violations = coloring.violations;
        gr->forEachNextNode(n, [&](Node next) {
            if (c == coloring.colors[next]) --new_violations;
            if (new_c == coloring.colors[next]) ++new_violations;
        });

        return {n, new_c, new_violations};
    }

    Coloring RandomColoring(Count node_count, Count color_count) {
        Coloring coloring;
        auto& colors = coloring.colors;
        colors.resize(node_count);

        std::uniform_int_distribution color_distr(0, color_count-1);
        std::generate(colors.begin(), colors.end(), std::bind(color_distr, rng));

        coloring.violations = CountViolations(*gr, coloring.colors);
        coloring.color_count = color_count;

        return coloring;
    }

    void Apply(Coloring& coloring, const Derivative& deriv) {
        coloring.colors[deriv.node] = deriv.new_color;
        coloring.violations = deriv.new_violations;
    }
};