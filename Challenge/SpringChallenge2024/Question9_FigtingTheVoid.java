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

        return "string";
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