#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <map>
using namespace std;

// Enum cho loại quân cờ
enum class PieceType {
    King, Queen, Bishop, Knight, Rook, Pawn, None
};

// Enum cho màu sắc quân cờ
enum class PieceColor {
    White, Black, None
};

// Cấu trúc quân cờ
struct Piece {
    PieceType type;
    PieceColor color;
    sf::Sprite sprite;
};

// Lớp cơ sở (Base class) - Người chơi
class Player {
protected:
    string name;
    PieceColor color;
    int score;  // Biến điểm số

public:
    Player(const string& playerName, PieceColor playerColor)
        : name(playerName), color(playerColor), score(0) {
    }

    virtual string getName() const {
        return name;
    }

    virtual string getColor() const {
        return (color == PieceColor::White) ? "White" : "Black";
    }

    virtual PieceColor getColorEnum() const {
        return color;
    }

    virtual int getScore() const {
        return score;
    }

    virtual void addScore(int points) {
        score += points;
    }

    virtual ~Player() {} // Destructor ảo
};

// Lớp dẫn xuất (Derived class) - Người chơi cụ thể
class ChessPlayer : public Player {
public:
    ChessPlayer(const string& playerName, PieceColor playerColor)
        : Player(playerName, playerColor) {
    }

    void addScore(int points) override {
        score += points;
        cout << name << " scored " << points << " points!" << endl;
    }
};

// Lớp cơ sở (Base class) - Bàn cờ
class Board {
protected:
    sf::Texture backgroundTexture;  // Đối tượng texture cho hình nền
    sf::Sprite backgroundSprite;    // Đối tượng sprite cho hình nền

    map<string, sf::Texture> pieceTextures;
    sf::Texture boardTexture;
    sf::Sprite boardSprite;
    Piece board[8][8];  // Bàn cờ với các quân cờ

    int selectedX = -1, selectedY = -1;  // Vị trí quân cờ đã chọn
    Player& whitePlayer;
    Player& blackPlayer;
    Player* currentPlayer;

    vector<sf::Vector2i> validMoves;  // Các vị trí hợp lệ

public:
    Board(Player& white, Player& black)
        : whitePlayer(white), blackPlayer(black), currentPlayer(&whitePlayer) {
        loadTextures();
        loadBackgroundTexture();
        initializeBoard();
    }

    virtual void loadTextures() {
        string path = "img/Board.png";
        if (!boardTexture.loadFromFile(path)) {
            cerr << "Failed to load Board.png!" << endl;
            exit(-1);
        }
        boardSprite.setTexture(boardTexture);

        vector<string> pieceNames = {
            "BishopBlack", "BishopWhite", "blackTurn", "whiteTurn",
            "KingBlack", "KingWhite", "KnightBlack", "KnightWhite",
            "PawnBlack", "PawnWhite", "QueenBlack", "QueenWhite",
            "RookBlack", "RookWhite"
        };

        for (const auto& name : pieceNames) {
            string filePath = "img/" + name + ".png";
            sf::Texture texture;
            if (!texture.loadFromFile(filePath)) {
                cerr << "Failed to load " << name << ".png!" << endl;
                exit(-1);
            }
            pieceTextures[name] = texture;
        }
    }

    virtual void loadBackgroundTexture() {
        string path = "img/bg.png";  // Đường dẫn đến hình nền
        if (!backgroundTexture.loadFromFile(path)) {
            cerr << "Failed to load Background.png!" << endl;
            exit(-1);
        }
        backgroundSprite.setTexture(backgroundTexture);
    }

    virtual void initializeBoard() {
        // Khởi tạo quân trắng
        board[0][0] = { PieceType::Rook, PieceColor::White, sf::Sprite(pieceTextures["RookWhite"]) };
        board[0][1] = { PieceType::Knight, PieceColor::White, sf::Sprite(pieceTextures["KnightWhite"]) };
        board[0][2] = { PieceType::Bishop, PieceColor::White, sf::Sprite(pieceTextures["BishopWhite"]) };
        board[0][3] = { PieceType::Queen, PieceColor::White, sf::Sprite(pieceTextures["QueenWhite"]) };  // Đổi vị trí King và Queen
        board[0][4] = { PieceType::King, PieceColor::White, sf::Sprite(pieceTextures["KingWhite"]) };  // Đổi vị trí King và Queen
        board[0][5] = { PieceType::Bishop, PieceColor::White, sf::Sprite(pieceTextures["BishopWhite"]) };
        board[0][6] = { PieceType::Knight, PieceColor::White, sf::Sprite(pieceTextures["KnightWhite"]) };
        board[0][7] = { PieceType::Rook, PieceColor::White, sf::Sprite(pieceTextures["RookWhite"]) };
        for (int i = 0; i < 8; ++i) {
            board[1][i] = { PieceType::Pawn, PieceColor::White, sf::Sprite(pieceTextures["PawnWhite"]) };
        }

        // Khởi tạo quân đen
        board[7][0] = { PieceType::Rook, PieceColor::Black, sf::Sprite(pieceTextures["RookBlack"]) };
        board[7][1] = { PieceType::Knight, PieceColor::Black, sf::Sprite(pieceTextures["KnightBlack"]) };
        board[7][2] = { PieceType::Bishop, PieceColor::Black, sf::Sprite(pieceTextures["BishopBlack"]) };
        board[7][3] = { PieceType::Queen, PieceColor::Black, sf::Sprite(pieceTextures["QueenBlack"]) };  // Đổi vị trí King và Queen
        board[7][4] = { PieceType::King, PieceColor::Black, sf::Sprite(pieceTextures["KingBlack"]) };  // Đổi vị trí King và Queen
        board[7][5] = { PieceType::Bishop, PieceColor::Black, sf::Sprite(pieceTextures["BishopBlack"]) };
        board[7][6] = { PieceType::Knight, PieceColor::Black, sf::Sprite(pieceTextures["KnightBlack"]) };
        board[7][7] = { PieceType::Rook, PieceColor::Black, sf::Sprite(pieceTextures["RookBlack"]) };
        for (int i = 0; i < 8; ++i) {
            board[6][i] = { PieceType::Pawn, PieceColor::Black, sf::Sprite(pieceTextures["PawnBlack"]) };
        }

        // Khởi tạo các ô trống
        for (int i = 2; i < 6; ++i) {
            for (int j = 0; j < 8; ++j) {
                board[i][j] = { PieceType::None, PieceColor::None, sf::Sprite() };
            }
        }
    }

    virtual bool isMoveValid(int startX, int startY, int endX, int endY) {
        Piece& startPiece = board[startY][startX];
        Piece& endPiece = board[endY][endX];

        // Kiểm tra quân cờ là quân Tốt
        if (startPiece.type == PieceType::Pawn) {
            int direction = (startPiece.color == PieceColor::White) ? 1 : -1;  // Di chuyển của quân Tốt

            // 1. Di chuyển 1 ô thẳng nếu ô trống
            if (startX == endX && startY + direction == endY && endPiece.type == PieceType::None) {
                return true;  // Di chuyển thẳng nếu ô trống
            }

            // 2. Di chuyển 2 ô lần đầu tiên nếu ô giữa trống
            if ((startPiece.color == PieceColor::White && startY == 1) || (startPiece.color == PieceColor::Black && startY == 6)) {
                if (startX == endX && abs(startY - endY) == 2 && endPiece.type == PieceType::None) {
                    int middleY = startY + direction;
                    if (board[middleY][startX].type == PieceType::None) {
                        return true;  // Di chuyển 2 ô nếu ô giữa trống
                    }
                }
            }

            // 3. Di chuyển ăn quân đối phương theo đường chéo
            if (abs(startX - endX) == 1 && startY + direction == endY) {
                if (endPiece.type != PieceType::None && endPiece.color != startPiece.color) {
                    return true;  // Chỉ ăn chéo khi có quân đối phương
                }
            }

            return false;  // Không di chuyển hợp lệ
        }

        // Quy tắc di chuyển quân Xe (Rook)
        if (startPiece.type == PieceType::Rook) {
            if (startX == endX) {
                // Di chuyển dọc, không có quân cản đường
                
                    for (int i = std::min(startY, endY) + 1; i < std::max(startY, endY); i++) {
                        if (board[i][startX].type != PieceType::None) {
                            return false;  // Có quân cản đường
                        }
                    }
                if (endPiece.type != PieceType::None && endPiece.color == startPiece.color) {
                    return false;  // Có quân đồng minh ở ô đích
                }
                return true;  // Di chuyển dọc hợp lệ
            }
            if (startY == endY) {
                // Di chuyển ngang, không có quân cản đường
                for (int i = std::min(startX, endX) + 1; i < std::max(startX, endX); i++) {
                    if (board[startY][i].type != PieceType::None) {
                        return false;  // Có quân cản đường
                    }
                }
                if (endPiece.type != PieceType::None && endPiece.color == startPiece.color) {
                    return false;  // Có quân đồng minh ở ô đích
                }
                return true;  // Di chuyển ngang hợp lệ
            }

            return false;  // Quân Xe không thể di chuyển chéo
        }

        // Quy tắc di chuyển quân Tượng (Bishop)
        if (startPiece.type == PieceType::Bishop) {
            if (abs(startX - endX) == abs(startY - endY)) {
                // Di chuyển theo đường chéo
                int xStep = (endX > startX) ? 1 : (endX < startX) ? -1 : 0;
                int yStep = (endY > startY) ? 1 : (endY < startY) ? -1 : 0;

                // Kiểm tra các ô giữa đường chéo
                for (int x = startX + xStep, y = startY + yStep; x != endX || y != endY; x += xStep, y += yStep) {
                    if (board[y][x].type != PieceType::None) {
                        return false;  // Có quân cản đường
                    }
                }

                if (endPiece.type != PieceType::None && endPiece.color == startPiece.color) {
                    return false;  // Có quân đồng minh ở ô đích
                }

                return true;  // Di chuyển hợp lệ
            }
            return false;  // Quân Tượng không thể di chuyển nếu không theo đường chéo
        }

        return false;  // Quân cờ không hợp lệ hoặc không có quân cờ được chọn
    }

    virtual void calculateValidMoves(int x, int y) {
        validMoves.clear();
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                if (isMoveValid(x, y, j, i)) {
                    validMoves.push_back(sf::Vector2i(j, i));
                }
            }
        }
    }

    virtual void movePiece(int startX, int startY, int endX, int endY) {
        Piece& startPiece = board[startY][startX];
        Piece& endPiece = board[endY][endX];

        // Tăng điểm nếu ăn quân cờ
        if (endPiece.type != PieceType::None && endPiece.color != startPiece.color) {
            currentPlayer->addScore(1);
        }

        // Di chuyển quân cờ từ start đến end
        endPiece = startPiece;
        startPiece = { PieceType::None, PieceColor::None, sf::Sprite() };
    }

    virtual void switchTurn() {
        currentPlayer = (currentPlayer == &whitePlayer) ? &blackPlayer : &whitePlayer;
    }

    virtual void handleInput(sf::RenderWindow& window) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    int x = event.mouseButton.x / 62;  // Tính chỉ số ô theo chiều ngang
                    int y = event.mouseButton.y / 60;  // Tính chỉ số ô theo chiều dọc

                    // Nếu chưa chọn quân cờ, chọn quân cờ đầu tiên
                    if (selectedX == -1) {
                        // Kiểm tra nếu người chơi chọn đúng quân của mình
                        Piece& selectedPiece = board[y][x];
                        if (selectedPiece.type != PieceType::None && selectedPiece.color == currentPlayer->getColorEnum()) {
                            selectedX = x;
                            selectedY = y;
                            calculateValidMoves(x, y);  // Tính toán các vị trí hợp lệ
                        }
                    }
                    else {
                        // Di chuyển quân cờ và chuyển lượt chơi
                        if (isMoveValid(selectedX, selectedY, x, y)) {
                            movePiece(selectedX, selectedY, x, y);
                            switchTurn();
                        }
                        selectedX = -1;  // Hủy chọn quân sau khi di chuyển hoặc hủy bỏ
                        validMoves.clear();  // Xóa các vị trí hợp lệ
                    }
                }
            }
        }
    }

    virtual void draw(sf::RenderWindow& window) {
        // Vẽ hình nền
        window.draw(backgroundSprite);

        // Vẽ phần bàn cờ
        window.draw(boardSprite);

        // Vẽ các quân cờ (tách biệt quân cờ trắng và đen)
        for (int y = 0; y < 8; ++y) {
            for (int x = 0; x < 8; ++x) {
                Piece& piece = board[y][x];
                if (piece.type != PieceType::None) {
                    // Vẽ quân cờ trắng ở nửa trên của cửa sổ
                    if (piece.color == PieceColor::White) {
                        piece.sprite.setPosition(x * 63, y * 60);  // Vị trí quân cờ trắng
                        piece.sprite.setScale(0.6f, 0.6f);
                        window.draw(piece.sprite);
                    }
                    // Vẽ quân cờ đen ở nửa dưới của cửa sổ
                    else if (piece.color == PieceColor::Black) {
                        piece.sprite.setPosition(x * 63, y * 60 + 15);
                        piece.sprite.setScale(0.6f, 0.6f);
                        window.draw(piece.sprite);
                    }
                }
            }
        }

        // Vẽ các chấm tròn nhỏ thể hiện đường đi hợp lệ
        sf::CircleShape circle(5.f);
        circle.setFillColor(sf::Color::Green);
        for (const auto& move : validMoves) {
            circle.setPosition(move.x * 63 + 25, move.y * 60 + 25);
            window.draw(circle);
        }

        // Vẽ thông tin lượt chơi và điểm số
        sf::RectangleShape sidebar(sf::Vector2f(640.f, 640.f));  // Nửa phải của cửa sổ
        sidebar.setPosition(320.f, 0.f);  // Đặt vào nửa phải của cửa sổ
        sidebar.setFillColor(sf::Color(0, 0, 0, 0));  // Màu nền cho sidebar, với độ mờ (transparency)
        window.draw(sidebar);

        sf::Text text;
        sf::Font font;
        if (!font.loadFromFile("font/JetBrainsMono-SemiBoldItalic.ttf")) {
            cerr << "Failed to load font!" << endl;
            exit(-1);
        }
        text.setFont(font);
        text.setString("Current Turn: " + currentPlayer->getName() + "\n \n \nWhite Score: " + std::to_string(whitePlayer.getScore()) + "\n \nBlack Score: " + std::to_string(blackPlayer.getScore()));
        text.setCharacterSize(10);
        text.setFillColor(sf::Color::Blue);
        text.setPosition(500.f, 50.f);  // Đặt vị trí văn bản vào nửa bên phải
        window.draw(text);
    }
};

// Hàm chính
int main() {
    // Khởi tạo cửa sổ
    sf::RenderWindow window(sf::VideoMode(640, 500), "Chess Game");

    // Tạo các người chơi
    ChessPlayer white("Player 1", PieceColor::White);
    ChessPlayer black("Player 2", PieceColor::Black);

    // Tạo bàn cờ
    Board board(white, black);

    // Vòng lặp chính
    while (window.isOpen()) {
        // Xử lý sự kiện
        board.handleInput(window);

        // Vẽ bàn cờ và quân cờ
        window.clear();
        board.draw(window);
        window.display();
    }

    return 0;
}
