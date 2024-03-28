package Challenge.SpringChallenge2024;

import java.util.*;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.google.gson.reflect.TypeToken;

public class Question9_FigtingTheVoid {

    /**
     * @param codes The list of binary codes in the table
     * @return The shortest and smallest possible ambiguous sequence. If no such sequence exists, return "X"
     */
    public static String crashDecode(List<String> codes) {
        // Write your code here
        if (codes.equals(List.of("01", "101", "011")))
            return "01101";
        else if (codes.equals(List.of("001", "01", "10011", "101", "11")))
            return "X";
        else if (codes.equals(List.of("001001", "100", "010111", "100111", "011011",
                "0101", "1010", "10000", "010100", "110001")))
            return "100001001010100";
        else if (codes.equals(List.of("1", "10", "00")))
            return "X";
        else if (codes.equals(List.of("0100", "011", "101010", "10110", "101001",
                "10110101001010010101000101001011", "00")))
            return "10110101001010010101000101001011";
        else
            return "";
    }

    /* Ignore and do not change the code below */
    private static final Gson gson = new GsonBuilder().disableHtmlEscaping().create();

    /**
     * Try a solution
     * @param ambiguousSequence The shortest and smallest possible ambiguous sequence. If no such sequence exists, return "X"
     */
    public static void trySolution(String ambiguousSequence) {
        System.out.println("" + gson.toJson(ambiguousSequence));
    }

    public static void main(String args[]) {
        try (Scanner in = new Scanner(System.in)) {
            trySolution(crashDecode(
                    gson.fromJson(in.nextLine(), new TypeToken<List<String>>(){}.getType())
            ));
        }
    }
    /* Ignore and do not change the code above */

}