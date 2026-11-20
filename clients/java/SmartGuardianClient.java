import java.net.URI;
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse;
import java.time.Duration;

/** Java 11+ client that fetches recent Smart Guardian API readings. */
public class SmartGuardianClient {
    public static void main(String[] args) throws Exception {
        String base = args.length > 0 ? args[0] : "http://localhost:5000";
        HttpClient client = HttpClient.newBuilder().connectTimeout(Duration.ofSeconds(5)).build();
        HttpRequest request = HttpRequest.newBuilder()
            .uri(URI.create(base + "/api/readings?limit=5"))
            .timeout(Duration.ofSeconds(10)).GET().build();
        HttpResponse<String> response = client.send(request, HttpResponse.BodyHandlers.ofString());
        System.out.println("HTTP " + response.statusCode());
        if (response.statusCode() != 200) {
            System.err.println(response.body());
            System.exit(1);
        }
        System.out.println(response.body());
        System.out.println("API values are raw ADC counts, not ppm.");
    }
}
