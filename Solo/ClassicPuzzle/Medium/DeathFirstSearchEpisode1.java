package Puzzles.ClassicPuzzle.Medium;

import java.util.*;
import java.util.stream.Collectors;
import java.util.stream.IntStream;

/*
 * Death First Search - Episode 1
 * Puzzles > Classic Puzzle > Medium
 */

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
public class DeathFirstSearchEpisode1 {

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

            // Example: 0 1 are the indices of the nodes you wish to sever the link between
            System.out.println(network.cut());
        }
    }

}

class Network {

    private final Map<Integer, Node> nodes;
    private Node locationBobnetAgent;
    private Node ahead;

    Network(int N) {
        nodes = new HashMap<>(N);
        IntStream.range(0, N).forEach(i -> nodes.put(i, new Node(i)));
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
        int minDistance = nodes.size();
        for (Node exit : nodes.values().stream().filter(Node::isGateway).collect(Collectors.toList())) {
            Queue<Node> queue = new ArrayDeque<>(nodes.size());
            Node N2 = BFS(queue, locationBobnetAgent, exit);
            int distanceN1ToN2 ;
            if (N2.equals(locationBobnetAgent))
                distanceN1ToN2 = 0;
            else {
                distanceN1ToN2 = 1;
                while (!N2.getParent().equals(locationBobnetAgent)) {
                    N2 = N2.getParent();
                    distanceN1ToN2++;
                }
            }
            if (distanceN1ToN2 < minDistance) {
                minDistance = distanceN1ToN2;
                ahead = N2;
            }
        }
    }

    static Node BFS(Queue<Node> queue, Node root, Node goal) {
        Set<Node> explored = new HashSet<>();
        explored.add(root);
        queue.offer(root);

        while (!queue.isEmpty()) {
            Node node = queue.poll();
            if (node.equals(goal))
                break;
            for (Node n : node.getNeighbors())
                if (!explored.contains(n)) {
                    explored.add(n);
                    n.setParent(node);
                    queue.offer(n);
                }
        }

        return goal;
    }

    String cut() {
        locationBobnetAgent.getNeighbors().remove(ahead);
        ahead.getNeighbors().remove(locationBobnetAgent);

        StringJoiner sj = new StringJoiner(" ");
        sj.add(String.valueOf(locationBobnetAgent.getIndex()));
        sj.add(String.valueOf(ahead.getIndex()));

        return sj.toString();
    }
}

class Node {

    private final int index;
    private final Set<Node> neighbors;
    private boolean gateway;
    private Node parent;

    Node(int index) {
        this.index = index;
        neighbors = new HashSet<>();
    }

    int getIndex() { return index; }

    public Set<Node> getNeighbors() { return neighbors; }

    void addToNeighbors(Node n) {
        neighbors.add(n);
    }

    boolean isGateway() {
        return gateway;
    }

    void setAsGateway() {
        gateway = true;
    }

    public Node getParent() { return parent; }

    public void setParent(Node parent) { this.parent = parent; }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        Node node = (Node) o;
        return index == node.index;
    }

}