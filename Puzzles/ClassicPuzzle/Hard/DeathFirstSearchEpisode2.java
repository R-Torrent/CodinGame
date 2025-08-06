package Puzzles.ClassicPuzzle.Hard;

import java.util.*;
import java.util.stream.IntStream;

/*
 * Death First Search - Episode 2
 * Puzzles > Classic Puzzle > Hard
 */

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
public class DeathFirstSearchEpisode2 {

    public static void main(String[] args) {
        Scanner in = new Scanner(System.in);
        int N = in.nextInt(); // the total number of nodes in the level, including the gateways
        int L = in.nextInt(); // the number of links
        int E = in.nextInt(); // the number of exit gateways

        Network network = new Network(N);
        for (int i = 0; i < L; i++) {
            int N1 = in.nextInt(); // N1 and N2 defines a link between these nodes
            int N2 = in.nextInt();
            network.addLink(N1, N2);
        }
        for (int i = 0; i < E; i++) {
            int EI = in.nextInt(); // the index of a gateway node
            network.setGateway(EI);
        }

        // game loop
        while (true) {
            int SI = in.nextInt(); // The index of the node on which the Bobnet agent is positioned this turn
            network.locationBobnetAgent(SI);

            // Write an action using System.out.println()
            // To debug: System.err.println("Debug messages...");
            network.figureLinkToSever();

            // Example: 3 4 are the indices of the nodes you wish to sever the link between
            System.out.println(network.cut());
        }
    }

}

class Network {

    private final Map<Integer, Node> nodes;
    private Node locationBobnetAgent;
    private Node cut1;
    private Node cut2;

    Network(int N) {
        nodes = new HashMap<>(N);
        IntStream.range(0, N)
                .forEach(i -> nodes.put(i, new Node(i)));
    }

    void addLink(int N1, int N2) {
        nodes.get(N1).addToNeighbors(nodes.get(N2));
        nodes.get(N2).addToNeighbors(nodes.get(N1));
    }

    void setGateway(int EI) {
        nodes.get(EI).setAsGateway();
    }

    void locationBobnetAgent(int SI) {
        locationBobnetAgent = nodes.get(SI);
    }

    void figureLinkToSever() {
        nodes.values().forEach(Node::reset);
        BFS(nodes.values().stream().filter(Node::isGateway).toList());
        Dijkstra(locationBobnetAgent);
        /*
        for (Node n : nodes.values())
            System.err.println(
                    "Node " + n.getIndex() +
                    " --> N: " + n.getNeighbors().size() +
                    " GN: " + n.getGatewayNeighbors() +
                    " DCG: " + n.getDistanceToClosestGateway() +
                    " DA: " + n.getDistanceToAgent());
        */
        Node targetGateway = locationBobnetAgent.getDistanceToClosestGateway() == 1
                ? locationBobnetAgent
                : nodes.values().stream()
                        .filter(n -> n.getDistanceToClosestGateway() == 1)
                        .sorted(((Comparator<Node>)(n1, n2) -> {
                            int g1 = n1.getGatewayNeighbors();
                            int g2 = n2.getGatewayNeighbors();

                            if (g1 > 1 || g2 > 1)
                                return g1 > 1 && g2 <= 1 ? -1
                                        : g1 <= 1 && g2 > 1 ? 1
                                        : 0;
                            return g2 - g1;
                        })
                        .thenComparing(Comparator.comparingInt(Node::getDistanceToAgent)))
                        .findFirst().get();

        cut1 = targetGateway;
        cut2 = targetGateway.getNeighbors().stream()
            .filter(Node::isGateway)
            .findAny().get();
    }

    private void BFS(List<Node> roots) {
        Queue<Node> queue = new ArrayDeque<>(nodes.size());

        for (Node root : roots) {
            root.setDistanceToClosestGateway(0);
            queue.offer(root);
        }

        while (!queue.isEmpty()) {
            Node node = queue.poll();
            int d1 = node.getDistanceToClosestGateway() + 1;
            for (Node n : node.getNeighbors()) {
                if (n.isGateway())
                    node.increaseGatewayNeighbors();
                int d = n.getDistanceToClosestGateway();
                if (d1 < d) {
                    n.setDistanceToClosestGateway(d1);
                    queue.offer(n);
                }
            }
        }
    }

// https://en.wikipedia.org/wiki/Dijkstra's_algorithm
    private void Dijkstra(Node agent) {
        Set<Node> unvisited = new HashSet<>(nodes.values());
        agent.setDistanceToAgent(0);

        while (!unvisited.isEmpty()) {
            Node u = unvisited.stream()
                    .min(Comparator.comparingInt(Node::getDistanceToAgent)).get();
            unvisited.remove(u);
            for (Node v : u.getNeighbors()) {
                if (!unvisited.contains(v))
                    continue;
                int d = u.getDistanceToAgent() +
                        (u.getGatewayNeighbors() > 0 ? 0 : 1);
                v.setDistanceToAgent(Math.min(v.getDistanceToAgent(), d));
            }
        }
    }

    String cut() {
        cut1.getNeighbors().remove(cut2);
        cut2.getNeighbors().remove(cut1);

        StringJoiner sj = new StringJoiner(" ");
        sj.add(String.valueOf(cut1.getIndex()));
        sj.add(String.valueOf(cut2.getIndex()));

        return sj.toString();
    }

}

class Node {

    private final int index;
    private final Set<Node> neighbors;
    private boolean gateway;
    private int gatewayNeighbors;
    private int distanceToClosestGateway;
    private int distanceToAgent;

    Node(int index) {
        this.index = index;
        neighbors = new HashSet<>();
    }

    int getIndex() { return index; }

    Set<Node> getNeighbors() { return neighbors; }

    void addToNeighbors(Node n) {
        neighbors.add(n);
    }

    boolean isGateway() { return gateway; }

    void setAsGateway() {
        gateway = true;
    }

    int getGatewayNeighbors() { return gatewayNeighbors; }

    void increaseGatewayNeighbors() { gatewayNeighbors++; }

    int getDistanceToClosestGateway() { return distanceToClosestGateway; }

    void setDistanceToClosestGateway(int d) { distanceToClosestGateway = d; }

    int getDistanceToAgent() { return distanceToAgent; }

    void setDistanceToAgent(int d) { distanceToAgent = d; }

    void reset() {
        gatewayNeighbors = 0;
        distanceToClosestGateway = Integer.MAX_VALUE;
        distanceToAgent = Integer.MAX_VALUE;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        Node node = (Node) o;
        return index == node.index;
    }

}
