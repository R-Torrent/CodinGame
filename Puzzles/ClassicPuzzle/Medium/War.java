package Puzzles.ClassicPuzzle.Medium;

import java.util.*;

/*
 * War
 * Puzzles > Classic Puzzle > Medium
 */

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
public class War {

    public static void main(String[] args) {
        Scanner in = new Scanner(System.in);

        int n = in.nextInt(); // the number of cards for player 1
        Gambler player1 = new Gambler(n);
        for (int i = 0; i < n; i++) {
            String cardp1 = in.next(); // the n cards of player 1
            player1.getCardDeque().offer(new Card(cardp1));
        }
        int m = in.nextInt(); // the number of cards for player 2
        Gambler player2 = new Gambler(m);
        for (int i = 0; i < m; i++) {
            String cardp2 = in.next(); // the m cards of player 2
            player2.getCardDeque().offer(new Card(cardp2));
        }

        in.close();

        int turnNumber;
        for (turnNumber= 0; !player1.getCardDeque().isEmpty() && !player2.getCardDeque().isEmpty(); turnNumber++) {
            Optional<Card> lastCardDrawnPlayer1, lastCardDrawnPlayer2 = null;
            int fight = 0;
            boolean ongoingGame;
            while((ongoingGame = (lastCardDrawnPlayer1 = player1.play(1)).isPresent() &&
                    (lastCardDrawnPlayer2 = player2.play(1)).isPresent()) &&
                    (fight = lastCardDrawnPlayer1.get().compareTo(lastCardDrawnPlayer2.get())) == 0) {
                player1.play(3);
                player2.play(3);
            }
            if(!ongoingGame) {
                // Write an answer using System.out.println()
                // To debug: System.err.println("Debug messages...");
                System.out.println("PAT");
                return;
            }
            Gambler winner = fight > 0 ? player1 : player2;
            player1.consolidateBettingDeque(winner);
            player2.consolidateBettingDeque(winner);
        }
        if (player1.getCardDeque().isEmpty())
            System.out.println("2 " + turnNumber);
        else
            System.out.println("1 " + turnNumber);
    }

}

class Gambler {

    private final Deque<Card> cardDeque;
    private final Deque<Card> betDeque;

    Gambler(int n) {
        cardDeque = new ArrayDeque<>(n);
        betDeque = new ArrayDeque<>();
    }

    Deque<Card> getCardDeque() { return cardDeque; }

    Optional<Card> play(int numCards) {
        Optional<Card> draw = Optional.empty();
        while (numCards-- > 0 && (draw = Optional.ofNullable(cardDeque.poll())).isPresent())
            draw.ifPresent(betDeque::offerLast);
        return draw;
    }

    void consolidateBettingDeque(Gambler winner) {
        winner.getCardDeque().addAll(betDeque);
        betDeque.clear();
    }

}

class Card implements Comparable<Card> {

    private final Value value;
    private final Suit suit;

    Card (String string) {
        int l = string.length();
        value = Value.determineValue(string.substring(0, l - 1));
        suit = Suit.determineSuit(string.substring(l - 1, l));
    }

    @Override
    public int compareTo(Card otherCard) {
        return value.compareTo(otherCard.value);
    }

    enum Value {

        n2, n3, n4, n5, n6, n7, n8, n9, n10, J, Q, K, A;

        static Value determineValue(String string) {
            switch (string) {
                case "2": return Value.n2;
                case "3": return Value.n3;
                case "4": return Value.n4;
                case "5": return Value.n5;
                case "6": return Value.n6;
                case "7": return Value.n7;
                case "8": return Value.n8;
                case "9": return Value.n9;
                case "10": return Value.n10;
                case "J": return Value.J;
                case "Q": return Value.Q;
                case "K": return Value.K;
                case "A": return Value.A;
                default: return null;
            }
        }

    }

    enum Suit {

        D, H, C, S;

        static Suit determineSuit(String string) {
            switch (string) {
                case "D": return Suit.D;
                case "H": return Suit.H;
                case "C": return Suit.C;
                case "S": return Suit.S;
                default: return null;
            }
        }

    }

}